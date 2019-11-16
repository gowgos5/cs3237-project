package com.example.android.bluetoothlegatt;

import android.content.Context;
import android.util.Log;
import android.widget.Toast;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.DisconnectedBufferOptions;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public class MqttHelper {

    // MQTT Information
    private static final String TAG = MqttHelper.class.getSimpleName();
    private static final String URI_SERVER = "tcp://192.168.43.42";
    private static final String ID_CLIENT = "iot-gateway";

    // List of MQTT Topics
    private static final String TOPIC_ACTIVITY = "iot/activity";
    public static final String TOPIC_PUBLISH_ACC = "iot/acc";
    public static final String TOPIC_PUBLISH_TEMP = "iot/temperature";
    public static final String TOPIC_PUBLISH_PPG = "iot/ppg";

    private final MqttAndroidClient client;

    public MqttHelper(final Context context) {
        client = new MqttAndroidClient(context, URI_SERVER, ID_CLIENT);
        client.setCallback(new MqttCallbackExtended() {
            @Override
            public void connectComplete(boolean b, String s) {
                Toast.makeText(context, "Successfully connected to the cloud!", Toast.LENGTH_SHORT).show();
            }

            @Override
            public void connectionLost(Throwable throwable) {
                Toast.makeText(context, "Lost connection to the cloud!", Toast.LENGTH_SHORT).show();
            }

            @Override
            public void messageArrived(String topic, MqttMessage mqttMessage) {
                if (topic.equals(TOPIC_ACTIVITY)){
                    ((PredictionActivity)context).updateActivity(mqttMessage.toString());
                }
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {
                // TODO: Message delivered
            }
        });

        connect(context);
    }

    private void connect(final Context context) {
        MqttConnectOptions mqttConnectOptions = new MqttConnectOptions();
        mqttConnectOptions.setAutomaticReconnect(true);
        mqttConnectOptions.setCleanSession(false);
        mqttConnectOptions.setKeepAliveInterval(60);

        try {
            client.connect(mqttConnectOptions, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    DisconnectedBufferOptions disconnectedBufferOptions = new DisconnectedBufferOptions();
                    disconnectedBufferOptions.setBufferEnabled(true);
                    disconnectedBufferOptions.setBufferSize(100);
                    disconnectedBufferOptions.setPersistBuffer(false);
                    disconnectedBufferOptions.setDeleteOldestMessages(false);
                    client.setBufferOpts(disconnectedBufferOptions);
                    subscribeToTopic(context);
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Log.d(TAG, "onFailure: " + exception.toString());
                }
            });
        } catch (MqttException ex) {
            ex.printStackTrace();
        }
    }

    public void subscribeToTopic(final Context context) {
        try {
            client.subscribe(TOPIC_ACTIVITY, 0, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    Toast.makeText(context, "Successful subscribe:" + TOPIC_ACTIVITY, Toast.LENGTH_SHORT).show();
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Toast.makeText(context, "Unsuccessful subscribe: " + TOPIC_ACTIVITY, Toast.LENGTH_SHORT).show();
                }
            });

        } catch (MqttException ex) {
            ex.printStackTrace();
        }
    }

    public void publish(String topic, String msg) {
        MqttMessage message = new MqttMessage(msg.getBytes());

        try {
            client.publish(topic, message);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }
}
