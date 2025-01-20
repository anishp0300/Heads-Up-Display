package com.puranish.bleblinkapp;

import android.app.Notification;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.Icon;
import android.os.Build;
import android.os.Bundle;
import android.service.notification.NotificationListenerService;
import android.service.notification.StatusBarNotification;
import android.util.Log;

import java.io.ByteArrayOutputStream;

import androidx.annotation.RequiresApi;

@RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN_MR2)

public class NotificationService extends NotificationListenerService {

    private Context context;
    private OnNotificationListener listener = null;

    public void setListener(OnNotificationListener listener) {
        this.listener = listener;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        context = getApplicationContext();
    }

    @RequiresApi(api = Build.VERSION_CODES.KITKAT)
    @Override
    public void onNotificationPosted(StatusBarNotification sbn) {
        super.onNotificationPosted(sbn);

        DeviceConnectedScreen.onNotification(sbn);

        /*String pack = sbn.getPackageName();
        String ticker = "";
        String title = "";
        String text = "";

        Bundle extras = sbn.getNotification().extras;
        Notification notification = sbn.getNotification();

        try{
            ticker = sbn.getNotification().tickerText.toString();
        }catch (NullPointerException e){
            e.getStackTrace();
        }
        try{
            title = extras.getString("android.title");
        }catch (NullPointerException e){
            e.getStackTrace();
        }
        try{
            CharSequence charSequence = extras.getCharSequence("android.text");
            if(charSequence!=null){
                text = charSequence.toString();
            }
        }catch (NullPointerException e){
            e.getStackTrace();
        }


        //Log.i("Package",pack);
        //Log.i("Ticker",ticker);
        //Log.i("Title",title);
        //Log.i("Text",text);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            Icon icon = notification.getSmallIcon();
            if(icon!=null){
                Drawable drawable = icon.loadDrawable(context);
                if(pack!=null){
                    Log.d("NOTIFICATION_MSG" , "BMP : " +drawable.getClass().getSimpleName());
                }
                if(drawable instanceof BitmapDrawable){
                    BitmapDrawable bmp = (BitmapDrawable) drawable;
                    Bitmap scaled = Bitmap.createScaledBitmap(bmp.getBitmap() , 24 , 24 , false);
                    ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
                    scaled.compress(Bitmap.CompressFormat.JPEG , 100 , outputStream);
                    //msgrcv.putExtra("icon" , outputStream.toByteArray());
                }
            }

        }

        //msgrcv.putExtra("package", pack);
        //msgrcv.putExtra("ticker", ticker);
        //msgrcv.putExtra("title", title);
        //msgrcv.putExtra("text", text);
        if(listener!=null){
            Log.d("NOTIFICATION_MSG", "Sending notification");
            listener.onNotification(pack , text , title);
        }else{
            Log.d("NOTIFICATION_MSG", "NULL");
        }

        //LocalBroadcastManager.getInstance(context).sendBroadcast(msgrcv);*/

    }

    @Override
    public void onNotificationRemoved(StatusBarNotification sbn) {
        super.onNotificationRemoved(sbn);
    }

    @Override
    public void onListenerConnected() {
        super.onListenerConnected();
        //Log.d("NOTIFICATION_MSG" , "CONNECTED!");
    }

    @Override
    public void onListenerDisconnected() {
        super.onListenerDisconnected();
        //Log.d("NOTIFICATION_MSG" , "CONNECTED!");
    }
}
