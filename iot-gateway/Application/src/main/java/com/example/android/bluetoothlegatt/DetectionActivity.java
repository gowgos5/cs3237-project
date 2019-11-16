package com.example.android.bluetoothlegatt;

import android.Manifest;
import android.app.Activity;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGattCharacteristic;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.Vibrator;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import androidx.core.app.ActivityCompat;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import static java.lang.String.valueOf;

public class DetectionActivity extends Activity implements LocationListener {
    private TextView textView;
    private LocationManager locationManager;
    private MqttHelper mqttHelper;
    protected Button mDeacButton;
    private MediaPlayer mp;

    /// define intent strings used the pass in data from other activities and services
    public static final String EXTRA_DEVICE = "extra_device";

    // BroadcastReceiver variables
    private boolean mConnected = false;
    private List<List<BluetoothGattCharacteristic>> mGattCharacteristics = new ArrayList<>();
    private BluetoothLeService mBluetoothLeService;
    private String mDeviceAddress;

    // UUIDs
    public final static String BLE_MAXIM_HSP_SERVICE = "5c6e40e8-3b7f-4286-a52f-daec46abe851"; // hsp service
    public final static String BLE_MAXIM_HSP_HEARTRATE_CHARACTERISTIC = "621a00e3-b093-46bf-aadc-abe4c648c569";  // heart rate

    // Vibrator
    private Vibrator v;

    //FFT
    private FftAnalyzer fftAnalyzer = new FftAnalyzer(12.5f, 10.f);
    private int step_count = 0;
    private float hr = -1;
    private boolean deacFlag = false;
    private static final int WINDOW_STEP_SIZE = 12;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_detection);
        mqttHelper = new MqttHelper(this);

        // BLE
        final Intent intent = getIntent();
        final BluetoothDevice device = intent.getParcelableExtra(EXTRA_DEVICE);
        mDeviceAddress = device.getAddress();

        v = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
        mp = new MediaPlayer();

        try
        {
            mp=MediaPlayer.create(getApplicationContext(), R.raw.buzzersound);
            mp.prepare();
            mp.setLooping(true);
        } catch (Exception e) {
            e.printStackTrace();
        }

        mDeacButton = findViewById(R.id.deacButton);
        mDeacButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                deactivationClick();
            }
        });

        textView = findViewById(R.id.coordText);
        locationManager = (LocationManager)getSystemService(Context.LOCATION_SERVICE);

        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION)
            != PackageManager.PERMISSION_GRANTED &&
            ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION)
            != PackageManager.PERMISSION_GRANTED){
            return;
        }

        Location location = locationManager.getLastKnownLocation(locationManager.NETWORK_PROVIDER);

        onLocationChanged(location);
    }

    @Override
    public void onLocationChanged(Location location) {
        double longitude = location.getLongitude();
        double latitude = location.getLatitude();
        textView.setText("Longitude: " + longitude + "\n" + "Latitude: " + latitude);

    }

    @Override
    public void onStatusChanged(String s, int i, Bundle bundle) {

    }

    @Override
    public void onProviderEnabled(String s) {

    }

    @Override
    public void onProviderDisabled(String s) {

    }

    public void updateHR(String hr){
        textView = findViewById(R.id.hrText);
        String toSet = "Heartrate: " + hr + " BPM";
        textView.setText(toSet);
        if (Integer.valueOf(hr) == 0 && !deacFlag) {
            updateStatus("Danger");
            v.vibrate(500);
            mp.start();
        }
    }

    public void updateStatus(String status){
        textView = findViewById(R.id.statusText);
        textView.setText(status);
        if (status.equals("Danger"))
            textView.setTextColor(Color.RED);
        else
            textView.setTextColor(Color.BLACK);
    }

    /**
     * Calculate the heart rate from the ECG RtoR device
     *
     * @param data Incoming data to convert
     * @return Returns a float representing the heart rate
     */
    private int CalculateHeartRate(byte[] data) {
        return ((short) data[0] + (short) (data[1] << 8) + (short) (data[2] << 16) + (short) (data[3] << 24));
    }

    /**
     * Subscribe to all of the HSP characteristics that we care about
     */
    private void subscribeCharacteristics() {
        for (List<BluetoothGattCharacteristic> chars : mGattCharacteristics) {
            for (BluetoothGattCharacteristic cha : chars) {
                if (cha.getService().getUuid().compareTo(UUID.fromString(BLE_MAXIM_HSP_SERVICE)) == 0) {
                    if (cha.getUuid().compareTo(UUID.fromString(BLE_MAXIM_HSP_HEARTRATE_CHARACTERISTIC)) == 0) {
                        mBluetoothLeService.Subscribe(cha.getService().getUuid(), cha.getUuid());
                    }
                }
            }
        }
    }

    /**
     * Called from the activity framework when the activity is paused
     */
    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(mGattUpdateReceiver);
    }

    /**
     * Called from the activity framework when the activity is resuming
     */
    @Override
    protected void onResume() {
        super.onResume();
        registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
        if (mBluetoothLeService != null) {
            final boolean result = mBluetoothLeService.connect(mDeviceAddress);
        }
    }

    /**
     * Define an intent filter of actions to respond to
     *
     * @return Returns the intent filter that is created
     */
    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_CONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(BluetoothLeService.ACTION_DATA_AVAILABLE);
        intentFilter.addAction(BluetoothLeService.ACTION_CHARACTERISTIC_WRITE);
        return intentFilter;
    }

    /**
     * Handles various events fired by the Service.
     */
    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(final Context context, final Intent intent) {
            final String action = intent.getAction();
            if (BluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                mConnected = true;
                invalidateOptionsMenu();
            } else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                mConnected = false;
                invalidateOptionsMenu();
                finish();   // get out of this activity and back to the parent
            } else if (BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
                subscribeCharacteristics();
            } else if (BluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {
                final String noData = getString(R.string.no_data);
                final String uuid = intent.getStringExtra(BluetoothLeService.EXTRA_UUID_CHAR);
                final byte[] dataArr = intent.getByteArrayExtra(BluetoothLeService.EXTRA_DATA_RAW);

                if (uuid != null) {
                    if (UUID.fromString(uuid).compareTo(UUID.fromString(BLE_MAXIM_HSP_HEARTRATE_CHARACTERISTIC)) == 0) {
                        long v = CalculateHeartRate(dataArr);
                        fftAnalyzer.addData((int) v);
                        step_count++;
                        if (step_count >= WINDOW_STEP_SIZE) {
                            step_count = 0;
                            hr = fftAnalyzer.getHeartRate();
                        }
                        updateHR(valueOf((int) hr));
                    }
                }
            } else if (BluetoothLeService.ACTION_CHARACTERISTIC_WRITE.equals(action)) {
                final String uuid = intent.getStringExtra(BluetoothLeService.EXTRA_UUID_CHAR);
            }
        }
    };

    /**
     * Go to Prediction page
     */
    private void deactivationClick() {
        updateStatus("Normal");
        mp.release(); // Stop buzzer sound
        deacFlag = true;
    }
}
