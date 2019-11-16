package com.example.android.bluetoothlegatt;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.text.format.DateUtils;
import android.widget.TextView;

import com.archit.calendardaterangepicker.customviews.DateRangeCalendarView;

import java.util.Calendar;

public class PredictionActivity extends Activity {
    private TextView textView;
    private MqttHelper mqttHelper;
    private DateRangeCalendarView calendarView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_prediction);

        calendarView = findViewById(R.id.calendar);
        calendarView.setSelectedDateRange(Calendar.getInstance(), Calendar.getInstance());

        calendarView.setCalendarListener(new DateRangeCalendarView.CalendarListener() {
            @Override
            public void onFirstDateSelected(Calendar startDate) {
                if (DateUtils.isToday(startDate.getTimeInMillis())) {
                    startActivity(new Intent(PredictionActivity.this, LongTermActivity.class));
                }

                calendarView.resetAllSelectedViews();
                calendarView.setSelectedDateRange(Calendar.getInstance(), Calendar.getInstance());
            }

            @Override
            public void onDateRangeSelected(Calendar startDate, Calendar endDate) {
            }
        });

        mqttHelper = new MqttHelper(this);
    }

    public void updateActivity(String text){
        textView = findViewById(R.id.activityText);
        textView.setText(text);
    }
}
