package com.example.android.bluetoothlegatt;

import android.app.Activity;
import android.os.Bundle;

import com.mikhaellopez.circularprogressbar.CircularProgressBar;

public class LongTermActivity extends Activity {
    CircularProgressBar cbActive;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_longterm);

        cbActive = findViewById(R.id.cbActive);
        cbActive.setProgressMax(60f);
        cbActive.setProgressWithAnimation(20f, 1000L);
    }
}
