package com.puranish.bleblinkapp;

import android.Manifest;
import android.app.Notification;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.Icon;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.ContactsContract;
import android.service.notification.StatusBarNotification;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import java.io.ByteArrayOutputStream;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.TimeZone;
import java.util.UUID;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.NotificationManagerCompat;

public class DeviceConnectedScreen extends AppCompatActivity implements OnNotificationListener{

    private static final String TAG = "BLE_SCAN";
    BluetoothAdapter adapter;
    BluetoothDevice bleDevice = null;
    private static BluetoothGatt bluetoothGatt = null;

    TextView titleTV;
    ProgressBar progressBar;
    TextView statusTV;
    Button button1,button2,button3,button4,button5,button6,button7,button8;

    static ArrayList<String> appList = new ArrayList<>();
    static Context context;


    @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN_MR2)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_device_connected_screen);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            getWindow().setStatusBarColor(getResources().getColor(R.color.colorPrimary));
        }

        context = getApplicationContext();

        Intent intent = getIntent();
        String address = intent.getStringExtra("BLE_DEVICE");
        if(address==null){
            Toast.makeText(getApplicationContext() , "Device Unavailable" , Toast.LENGTH_LONG).show();
            finish();
            return;
        }

        //appList.add("com.whatsapp");
        new NotificationService().setListener(this);

        titleTV = findViewById(R.id.device_name_title_tv);
        progressBar = findViewById(R.id.scan_device_pb);
        statusTV = findViewById(R.id.scan_device_status_tv);

        button1 = findViewById(R.id.btn1);
        button2 = findViewById(R.id.btn2);
        button3 = findViewById(R.id.btn3);
        button4 = findViewById(R.id.btn4);
        button5 = findViewById(R.id.btn5);
        button6 = findViewById(R.id.btn6);
        button7 = findViewById(R.id.btn7);
        button8 = findViewById(R.id.btn8);

        adapter = BluetoothAdapter.getDefaultAdapter();
        bleDevice = adapter.getRemoteDevice(address);
        titleTV.setText(bleDevice.getName());

        button1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(bluetoothGatt==null){
                    Toast.makeText(getApplicationContext() , "Reconnect Device!" , Toast.LENGTH_LONG).show();
                    return;
                }


                long l = Calendar.getInstance().getTime().getTime() + TimeZone.getDefault().getRawOffset();
                Log.d(TAG, "SENDING : " + l);
                byte[] arr = new byte[9];
                arr[0] = 'T';
                arr[1] = (byte)l;
                arr[2] = (byte)(l >> 8);
                arr[3] = (byte)(l >> 16);
                arr[4] = (byte)(l >> 24);
                arr[5] = (byte)(l >> 32);
                arr[6] = (byte)(l >> 40);
                arr[7] = (byte)(l >> 48);
                arr[8] = (byte)(l >> 56);
                writeCharacteristic(bluetoothGatt , arr);

                Toast.makeText(getApplicationContext() , "DataPacket Sent!" , Toast.LENGTH_LONG).show();

            }
        });

        button2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(bluetoothGatt==null){
                    Toast.makeText(getApplicationContext() , "Reconnect Device!" , Toast.LENGTH_LONG).show();
                    return;
                }

                byte[] arr = new byte[11];
                arr[0] = 'C';
                arr[1] = 'S';
                arr[2] = 'a';
                arr[3] = 'n';
                arr[4] = 'i';
                arr[5] = 'k';
                arr[6] = 'a';
                arr[7] = ' ';
                arr[8] = 'J';
                arr[9] = 'i';
                arr[10] = 'o';
                writeCharacteristic(bluetoothGatt , arr);

                Toast.makeText(getApplicationContext() , "DataPacket Sent!" , Toast.LENGTH_LONG).show();

            }
        });

        button3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(bluetoothGatt==null){
                    Toast.makeText(getApplicationContext() , "Reconnect Device!" , Toast.LENGTH_LONG).show();
                    return;
                }

                byte[] arr = new byte[11];
                arr[0] = 'C';
                arr[1] = '7';
                arr[2] = '7';
                arr[3] = '9';
                arr[4] = '9';
                arr[5] = '6';
                arr[6] = '5';
                arr[7] = '3';
                arr[8] = '0';
                arr[9] = '8';
                arr[10] = '3';
                writeCharacteristic(bluetoothGatt , arr);

                Toast.makeText(getApplicationContext() , "DataPacket Sent!" , Toast.LENGTH_LONG).show();

            }
        });

        button4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(bluetoothGatt==null){
                    Toast.makeText(getApplicationContext() , "Reconnect Device!" , Toast.LENGTH_LONG).show();
                    return;
                }

                String name = "Sanika Jio";
                String msg = "Hello... Sending you a big text message for test. Call me back as soon as possible! Have a good day!!!   Bye!! Good Night!";
                byte[] arr = new byte[name.length() + msg.length() + 3];
                arr[0] = 'S';
                int i = 1;
                for(char c : name.toCharArray()){
                    arr[i++] = (byte) c;
                }
                arr[i++] = '\0';

                for(char c : msg.toCharArray()){
                    arr[i++] = (byte) c;
                }
                arr[i] = '\0';
                writeCharacteristic(bluetoothGatt , arr);

                Toast.makeText(getApplicationContext() , "DataPacket Sent!" , Toast.LENGTH_LONG).show();

            }
        });

        button5.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(bluetoothGatt==null){
                    Toast.makeText(getApplicationContext() , "Reconnect Device!" , Toast.LENGTH_LONG).show();
                    return;
                }

                String name = "7769953083";
                String msg = "HELLO WORLD! TEST!!! How Are you? Call back ASAP!";
                byte[] arr = new byte[name.length() + msg.length() + 3];
                arr[0] = 'S';
                int i = 1;
                for(char c : name.toCharArray()){
                    arr[i++] = (byte) c;
                }
                arr[i++] = '\0';

                for(char c : msg.toCharArray()){
                    arr[i++] = (byte) c;
                }
                arr[i] = '\0';
                writeCharacteristic(bluetoothGatt , arr);

                Toast.makeText(getApplicationContext() , "DataPacket Sent!" , Toast.LENGTH_LONG).show();

            }
        });

        button6.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String msg = "130m";
                byte[] img = new byte[]{0, 0, 0, 0, 0, 0, 1, -128, 0, 3, -128, 0, 7, -128, 0, 15, 0, 0, 30, 0, 0, 63, -1, -32, 127, -1, -8, 127, -1, -4, 62, 0, 28, 31, 0, 14, 7, -128, 14, 3, -128, 14, 1, -128, 14, 0, 0, 14, 0, 0, 14, 0, 0, 14, 0, 0, 14, 0, 0, 14, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                byte[] arr = new byte[73 + msg.length()];
                arr[0] = 'M';
                System.arraycopy(img, 0, arr, 1, 72);
                int i = 73;
                for(char c : msg.toCharArray()){
                    arr[i++] = (byte) c;
                }
                writeCharacteristic(bluetoothGatt , arr);
                Toast.makeText(getApplicationContext() , "DataPacket Sent!" , Toast.LENGTH_LONG).show();
            }
        });

        button7.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String msg = "90m";
                byte[] img = new byte[]{0, 0, 0, 0, 112, 0, 3, -4, 0, 7, -2, 0, 15, -97, 0, 30, 7, -128, 28, 3, -128, 60, 3, -128, 56, 1, -64, 56, 1, -64, 56, 1, -60, 56, 57, -50, 56, 61, -34, 56, 31, -4, 56, 15, -8, 56, 15, -16, 56, 7, -32, 56, 3, -64, 56, 1, -128, 56, 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                byte[] arr = new byte[73 + msg.length()];
                arr[0] = 'M';
                System.arraycopy(img, 0, arr, 1, 72);
                int i = 73;
                for(char c : msg.toCharArray()){
                    arr[i++] = (byte) c;
                }
                writeCharacteristic(bluetoothGatt , arr);
                Toast.makeText(getApplicationContext() , "DataPacket Sent!" , Toast.LENGTH_LONG).show();
            }
        });

        button8.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String msg = "1.2km";
                byte[] img = new byte[]{0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 60, 0, 0, 126, 0, 0, -1, 0, 1, -1, -128, 3, -37, -64, 7, -103, -32, 3, 24, -64, 0, 24, 0, 0, 24, 0, 0, 24, 0, 0, 24, 0, 0, 24, 0, 0, 0, 0, 0, 24, 0, 0, 24, 0, 0, 0, 0, 0, 24, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                byte[] arr = new byte[73 + msg.length()];
                arr[0] = 'M';
                System.arraycopy(img, 0, arr, 1, 72);
                int i = 73;
                for(char c : msg.toCharArray()){
                    arr[i++] = (byte) c;
                }
                writeCharacteristic(bluetoothGatt , arr);
                Toast.makeText(getApplicationContext() , "DataPacket Sent!" , Toast.LENGTH_LONG).show();
            }
        });

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(new String[]{Manifest.permission.BIND_NOTIFICATION_LISTENER_SERVICE} , 0);
        }

        boolean n = NotificationManagerCompat.getEnabledListenerPackages (getApplicationContext()).contains(getApplicationContext().getPackageName());

        if(!n){
            startActivity(new Intent("android.settings.ACTION_NOTIFICATION_LISTENER_SETTINGS").addFlags(Intent.FLAG_ACTIVITY_NEW_TASK));

        }


        connect();

    }

    @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN_MR2)
    private void connect() {
        if(bleDevice!=null){

            Log.d(TAG , "CONNECTING GATT");
            bluetoothGatt = bleDevice.connectGatt(getApplicationContext(), true, new BluetoothGattCallback() {
                @Override
                public void onPhyUpdate(BluetoothGatt gatt, int txPhy, int rxPhy, int status) {
                    super.onPhyUpdate(gatt, txPhy, rxPhy, status);
                }

                @Override
                public void onPhyRead(BluetoothGatt gatt, int txPhy, int rxPhy, int status) {
                    super.onPhyRead(gatt, txPhy, rxPhy, status);
                }

                @Override
                public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
                    super.onConnectionStateChange(gatt, status, newState);
                    Log.d(TAG , "STATUS : " + status);
                    Log.d(TAG , "NEW STATE : " + newState);

                    if(newState == BluetoothProfile.STATE_CONNECTED){
                        Log.d(TAG , "CONNECTED & DISCOVERING SERVICES");
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                statusTV.setText(R.string.connected);
                                progressBar.setVisibility(View.GONE);
                                button1.setEnabled(true);
                                button2.setEnabled(true);
                                button3.setEnabled(true);
                                button4.setEnabled(true);
                                button5.setEnabled(true);
                                button6.setEnabled(true);
                                button7.setEnabled(true);
                                button8.setEnabled(true);
                                button1.setAlpha(1.0f);
                                button2.setAlpha(1.0f);
                                button3.setAlpha(1.0f);
                                button4.setAlpha(1.0f);
                                button5.setAlpha(1.0f);
                                button6.setAlpha(1.0f);
                                button7.setAlpha(1.0f);
                                button8.setAlpha(1.0f);
                            }
                        });
                        gatt.discoverServices();
                    }else if(newState == BluetoothProfile.STATE_CONNECTING){
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                statusTV.setText(R.string.connecting);
                                progressBar.setVisibility(View.VISIBLE);
                                button1.setEnabled(false);
                                button2.setEnabled(false);
                                button3.setEnabled(false);
                                button4.setEnabled(false);
                                button5.setEnabled(false);
                                button6.setEnabled(false);
                                button7.setEnabled(false);
                                button8.setEnabled(false);
                                button1.setAlpha(0.3f);
                                button2.setAlpha(0.3f);
                                button3.setAlpha(0.3f);
                                button4.setAlpha(0.3f);
                                button5.setAlpha(0.3f);
                                button6.setAlpha(0.3f);
                                button7.setAlpha(0.3f);
                                button8.setAlpha(0.3f);
                            }
                        });
                    }else if(newState == BluetoothProfile.STATE_DISCONNECTED){
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                statusTV.setText(R.string.disconnected);
                                progressBar.setVisibility(View.GONE);
                                button1.setEnabled(false);
                                button2.setEnabled(false);
                                button3.setEnabled(false);
                                button4.setEnabled(false);
                                button5.setEnabled(false);
                                button6.setEnabled(false);
                                button7.setEnabled(false);
                                button8.setEnabled(false);
                                button1.setAlpha(0.3f);
                                button2.setAlpha(0.3f);
                                button3.setAlpha(0.3f);
                                button4.setAlpha(0.3f);
                                button5.setAlpha(0.3f);
                                button6.setAlpha(0.3f);
                                button7.setAlpha(0.3f);
                                button8.setAlpha(0.3f);
                            }
                        });
                    }

                }

                @Override
                public void onServicesDiscovered(BluetoothGatt gatt, int status) {
                    super.onServicesDiscovered(gatt, status);
                    Log.d(TAG , "SERVICES DISCOVERED : " + gatt.getServices().size());
                }

                @Override
                public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
                    super.onCharacteristicRead(gatt, characteristic, status);
                    //final  byte[] data = characteristic.getValue();
                    /*if(data.length > 0){
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                textView.setText(R.string.led_is);
                                textView.append(data[0] == 0 ? " OFF" : " ON");
                            }
                        });

                    }*/
                }

                @Override
                public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
                    super.onCharacteristicWrite(gatt, characteristic, status);
                    //readCharacteristic();
                }

                @Override
                public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
                    super.onCharacteristicChanged(gatt, characteristic);
                }

                @Override
                public void onDescriptorRead(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
                    super.onDescriptorRead(gatt, descriptor, status);
                }

                @Override
                public void onDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
                    super.onDescriptorWrite(gatt, descriptor, status);
                }

                @Override
                public void onReliableWriteCompleted(BluetoothGatt gatt, int status) {
                    super.onReliableWriteCompleted(gatt, status);
                }

                @Override
                public void onReadRemoteRssi(BluetoothGatt gatt, int rssi, int status) {
                    super.onReadRemoteRssi(gatt, rssi, status);
                }

                @Override
                public void onMtuChanged(BluetoothGatt gatt, int mtu, int status) {
                    super.onMtuChanged(gatt, mtu, status);
                }
            });

            //gatt.getServices().add(new BluetoothGattService(UUID.fromString("00001523-1212-efde-1523-785feabcd123") , BluetoothGattService.SERVICE_TYPE_PRIMARY));

        }


    }

    @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN_MR2)
    private static void  writeCharacteristic(BluetoothGatt gatt , byte[] data){

        BluetoothGattService service = gatt.getService(UUID.fromString("6e400001-b5a3-f393-e0a9-e50e24dcca9e"));
        if(service!=null){
            BluetoothGattCharacteristic characteristic = service.getCharacteristic(UUID.fromString("6e400002-b5a3-f393-e0a9-e50e24dcca9e"));
            if(characteristic!=null){
                characteristic.setValue(data);
                gatt.writeCharacteristic(characteristic);
            }else{
                Log.d(TAG , "Led chars not found!");
            }
        }else {
            Log.d(TAG , "LBS Service not found!");
        }

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onNotification(String packageName, String text, String title) {
        Log.d("NOTIFICATION_LISTENER" , packageName);
    }

    static class MyPhoneStateListener extends PhoneStateListener {


        private final Context context;
        private boolean isCallShown = false;

        public MyPhoneStateListener(Context context) {
            this.context = context;
        }

        public void onCallStateChanged(int state, String incomingNumber) {

            if (state == TelephonyManager.CALL_STATE_RINGING) {

                if(isCallShown){
                    return;
                }

                String name = getContactName(context , incomingNumber);
                if(name!=null){
                    if(bluetoothGatt==null){
                        Toast.makeText(context , "Reconnect Device!" , Toast.LENGTH_LONG).show();
                        return;
                    }

                    int len = 0;
                    for(char c : name.toCharArray()){
                        if((int)c < 255){
                            len++;
                        }
                    }

                    byte[] arr = new byte[len + 1];

                    arr[0] = 'C';
                    for(int i = 1 ; i < arr.length ; i++){
                        int a = name.charAt(i - 1);
                        if(a<255){
                            arr[i] = (byte)a;
                        }
                    }
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
                        writeCharacteristic(bluetoothGatt , arr);
                        isCallShown = true;
                    }

                    Toast.makeText(context , "DataPacket Sent!" , Toast.LENGTH_LONG).show();
                }
                //Log.d("INCOMING_CALL" , "" + getContactName(context , incomingNumber));

            }else if(state == TelephonyManager.CALL_STATE_OFFHOOK || state == TelephonyManager.CALL_STATE_IDLE){
                isCallShown = false;
            }
        }



    }

    public static String getContactName(Context context , final String phoneNumber) {
        Uri uri = Uri.withAppendedPath(ContactsContract.PhoneLookup.CONTENT_FILTER_URI,Uri.encode(phoneNumber));
        String[] projection = new String[]{ContactsContract.PhoneLookup.DISPLAY_NAME};
        Cursor cursor = context.getContentResolver().query(uri,projection,null,null,null);

        if (cursor != null) {
            if(cursor.moveToFirst()) {
                return cursor.getString(0);
            }
            cursor.close();
        }

        return null;
    }

    static void onSMS(Context context , String number , String body){
        Log.d("SMS_LISTENER" , "FROM : " + getContactName(context , number));
        Log.d("SMS_LISTENER" , "BODY : " + body);
        if(bluetoothGatt==null){
            Toast.makeText(context , "Reconnect Device!" , Toast.LENGTH_LONG).show();
            return;
        }

        String name = getContactName(context , number);

        byte[] arr = new byte[name.length() + body.length() + 3];
        arr[0] = 'S';
        int i = 1;
        for(char c : name.toCharArray()){
            arr[i++] = (byte) c;
        }
        arr[i++] = '\0';

        for(char c : body.toCharArray()){
            arr[i++] = (byte) c;
        }
        arr[i] = '\0';
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
            writeCharacteristic(bluetoothGatt , arr);
        }

        Toast.makeText(context , "DataPacket Sent!" , Toast.LENGTH_LONG).show();
    }

    static void onNotification(StatusBarNotification sbn){
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
            String packageName = sbn.getPackageName().toLowerCase();

            if(packageName.contains("com.google.android.apps.maps")){
                Notification notification = sbn.getNotification();
                if(notification!=null){
                    if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.KITKAT) {
                        Bundle extras = notification.extras;
                        if(extras!=null){
                            CharSequence text = extras.getCharSequence("android.text");
                            CharSequence title = extras.getCharSequence("android.title");
                            Bitmap navIcon = null;
                            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M) {
                                Icon icon = notification.getLargeIcon();
                                if(icon!=null && context!=null){
                                    Drawable drawable = icon.loadDrawable(context);

                                    if(drawable instanceof BitmapDrawable){
                                        BitmapDrawable bmp = (BitmapDrawable) drawable;
                                        navIcon = Bitmap.createScaledBitmap(bmp.getBitmap() , 24 , 24 , false);
                                        ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
                                        navIcon.compress(Bitmap.CompressFormat.JPEG , 100 , outputStream);
                                    }

                                    if(text!=null){
                                        Log.d("NAVIGATION_MODE" , text.toString());
                                    }else {
                                        Log.d("NAVIGATION_MODE" , "NULL TEXT");
                                    }

                                    if(title!=null){
                                        Log.d("NAVIGATION_MODE" , title.toString());
                                    }else {
                                        Log.d("NAVIGATION_MODE" , "NULL TEXT");
                                    }

                                    if(text!=null && title!=null && navIcon!=null){


                                        String msg = title.toString();
                                        byte[] img = getImageArray(navIcon);

                                        byte[] arr = new byte[73 + msg.length()];
                                        arr[0] = 'M';
                                        System.arraycopy(img, 0, arr, 1, 72);
                                        int i = 73;
                                        for(char c : msg.toCharArray()){
                                            arr[i++] = (byte) c;
                                        }
                                        writeCharacteristic(bluetoothGatt , arr);
                                    }else {
                                        if(navIcon == null){
                                            Log.d("NAVIGATION_MODE" , "NULL NAV ICON");
                                        }
                                    }
                                }else{
                                    Log.d("NAVIGATION_MODE" , "ICON OR CONTEXT NULL");
                                }

                            }

                        }else {
                            Log.d("NAVIGATION_MODE" , "NULL EXTRAS");
                        }
                    }
                }else{
                    Log.d("NAVIGATION_MODE" , "NULL NOTIFICATION");
                }
            }else{
                if(!appList.contains(packageName)){
                    Log.d("NOTIFICATION_MSG" , sbn.getPackageName());

                    Notification notification = sbn.getNotification();
                    if(notification!=null){
                        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.KITKAT) {
                            Bundle extras = notification.extras;
                            if(extras!=null){
                                CharSequence text = extras.getCharSequence("android.text");
                                CharSequence title = extras.getCharSequence("android.title");
                                Bitmap appIcon = null;
                                if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M) {
                                    Icon icon = notification.getLargeIcon();
                                    if(icon!=null && context!=null){
                                        Drawable drawable = icon.loadDrawable(context);

                                        if(drawable!=null){
                                            appIcon = Bitmap.createScaledBitmap(getBitmapFromDrawable(drawable) , 24 , 24 , false);
                                            ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
                                            appIcon.compress(Bitmap.CompressFormat.JPEG , 100 , outputStream);
                                        }else{
                                            Log.d("NOTIFICATION_MSG" , "NULL DRAWABLE");
                                        }

                                        if(text!=null){
                                            Log.d("NOTIFICATION_MSG" , text.toString());
                                        }else {
                                            Log.d("NOTIFICATION_MSG" , "NULL TEXT");
                                        }

                                        if(title!=null){
                                            Log.d("NOTIFICATION_MSG" , title.toString());
                                        }else {
                                            Log.d("NOTIFICATION_MSG" , "NULL TEXT");
                                        }

                                        if(text!=null && title!=null && appIcon!=null){


                                            String msg = title.toString();
                                            byte[] img = getImageArray(appIcon);

                                            byte[] arr = new byte[73 + msg.length()];
                                            arr[0] = 'N';
                                            System.arraycopy(img, 0, arr, 1, 72);
                                            int i = 73;
                                            for(char c : msg.toCharArray()){
                                                arr[i++] = (byte) c;
                                            }
                                            writeCharacteristic(bluetoothGatt , arr);
                                        }else {
                                            if(appIcon == null){
                                                Log.d("NOTIFICATION_MSG" , "NULL NAV ICON");
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                }else{
                    Log.d("NOTIFICATION_MSG" , "NO : " + sbn.getPackageName());
                }
            }
        }
    }

    static byte[] getImageArray(Bitmap bitmap){
        byte[] img = new byte[72];
        for(int x = 0 ; x < 72 ; x++){
            img[x] = 0;
        }

        int index = 0;
        int shift = 0;
        for(int y = 0 ; y < 24 ; y++){
            for(int x = 0 ; x < 24 ; x++){
                int pixel = bitmap.getPixel(x , y);
                if(pixel == Color.WHITE){
                    img[index] |= (1 << (7 - shift));
                }else{
                    img[index] &= ~(1 << (7 - shift));
                }
                shift++;
                if(shift >= 8){
                    shift = 0;
                    index++;
                }
            }
        }
        return img;
    }

    static Bitmap getBitmapFromDrawable(@NonNull Drawable drawable) {
        final Bitmap bmp = Bitmap.createBitmap(drawable.getIntrinsicWidth(), drawable.getIntrinsicHeight(), Bitmap.Config.ARGB_8888);
        final Canvas canvas = new Canvas(bmp);
        drawable.setBounds(0, 0, canvas.getWidth(), canvas.getHeight());
        drawable.draw(canvas);
        return bmp;
    }

}