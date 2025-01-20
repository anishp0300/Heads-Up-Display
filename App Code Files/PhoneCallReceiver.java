package com.puranish.bleblinkapp;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;

public class PhoneCallReceiver extends BroadcastReceiver {

    //The receiver will be recreated whenever android feels like it.  We need a static variable to remember data between instantiations

    private static final String TAG = "INCOMING_CALL";

    @Override
    public void onReceive(Context context, Intent intent) {

        if(intent.getAction()!=null && intent.getAction().equals("android.intent.action.PHONE_STATE")) {

            TelephonyManager telephonyManager = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
            DeviceConnectedScreen.MyPhoneStateListener PhoneListener = new DeviceConnectedScreen.MyPhoneStateListener(context);
            telephonyManager.listen(PhoneListener, PhoneStateListener.LISTEN_CALL_STATE);
        }

    }


}
