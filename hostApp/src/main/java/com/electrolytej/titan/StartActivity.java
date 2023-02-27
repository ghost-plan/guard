package com.electrolytej.titan;

import android.content.Intent;
import android.os.Bundle;
import android.os.RemoteException;
import android.view.View;
import android.widget.Button;

import androidx.annotation.Nullable;

import com.jamesfchen.binder.Binder;
import com.jamesfchen.binder.ServiceManager;
import com.electrolytej.guard.TestGuardActivity;

/**
 * Copyright ® $ 2017
 * All right reserved.
 *
 * @author: hawks.jamesf
 * @since: Oct/25/2020  Sun
 */
public class StartActivity extends AbsPermissionsActivity {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_start);
        Button button = findViewById(R.id.bt);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(StartActivity.this, TestGuardActivity.class));
//                Router.select("module1_page").go(StartActivity.this,new Callback(){
//
//                });
            }
        });
        try {
            ServiceManager.getInstance().addService("first",new Binder());
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onRequestPermissionsResult() {
//        startActivity(new Intent(this, TestGuardActivity.class));
//        startActivity(new Intent(this, YPosedActivity.class));
        startActivity(new Intent(this, MainActivity.class));
    }

    @Override
    protected void onStart() {
        super.onStart();
//        long start = System.currentTimeMillis();
        for (int i = 0; i < 5; ++i) {
            try {
                Thread.sleep(200);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
//        Log.d("cjf", "耗时:" + (System.currentTimeMillis() - start));

    }

    @Override
    protected void onResume() {
        super.onResume();
        for (int i = 0; i < 5; ++i) {
            try {
                Thread.sleep(200);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }
}
