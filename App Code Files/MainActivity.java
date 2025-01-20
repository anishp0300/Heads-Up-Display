package com.puranish.bleblinkapp;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ProgressBar;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {


    RecyclerView recyclerView;
    ProgressBar progressBar;

    private static final String TAG = "BLE_SCAN";

    BluetoothAdapter adapter;

    ScanDeviceAdapter listAdapter;

    long scanTime = 0;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            getWindow().setStatusBarColor(getResources().getColor(R.color.colorPrimary));
        }

        recyclerView = findViewById(R.id.recyclerView);
        progressBar = findViewById(R.id.progressBar);

        recyclerView.setHasFixedSize(false);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));

        adapter = BluetoothAdapter.getDefaultAdapter();

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(new String[]{Manifest.permission.ACCESS_COARSE_LOCATION , Manifest.permission.READ_PHONE_STATE} , 6968);
        }


    }

    protected void continueOnCreate(){
        final BluetoothLeScanner scanner;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            scanner = adapter.getBluetoothLeScanner();

            final ScanCallback scanCallback = new ScanCallback() {
                @Override
                public void onScanResult(int callbackType, ScanResult result) {
                    BluetoothDevice device = result.getDevice();
                    if(device!=null){
                        String name = device.getName();
                        if(name!=null){
                            /*if(name.equals("Nordic_UART")){
                                Log.d(TAG , "UART DETECTED!");
                                if(bleDevice==null){
                                    bleDevice = device;
                                    bleResult = result;
                                    scanner.stopScan(this);
                                    Log.d(TAG , "STOPPING SCAN AND INIT CONNECT");
                                    connect();
                                }
                                BluetoothClass bluetoothClass = device.getBluetoothClass();
                                if(bluetoothClass!=null){
                                    Log.d(TAG , bluetoothClass.getDeviceClass() + "");
                                }else{
                                    Log.d(TAG , "CLASS NOT FOUND!");
                                }
                                ScanRecord record = result.getScanRecord();
                                if(record!=null){
                                    List<ParcelUuid> uuidList = record.getServiceUuids();
                                    if(uuidList!=null){
                                        Log.d(TAG , "SERVICE UUIDs :");
                                        for(ParcelUuid uuid : uuidList){
                                            Log.d(TAG , uuid.toString());
                                        }
                                    }else{
                                        Log.d(TAG , "NO SERVICE UUIDs");
                                    }
                                }else{
                                    Log.d(TAG , "NO SCAN RECORD");
                                }
                            }*/

                            if(!containsDevice(device)){
                                listAdapter.getDataset().add(device);
                                runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        listAdapter.notifyDataSetChanged();
                                    }
                                });

                            }


                        }
                        //Log.d(TAG , name != null ? name : device.getAddress()!=null ? device.getAddress() : "UNKNOWN");
                    }else{
                        Log.d(TAG , "NULL DEVICE");
                    }

                    if(System.currentTimeMillis() - scanTime > 10000){
                        scanner.stopScan(this);
                        progressBar.setVisibility(View.GONE);
                        Toast.makeText(getApplicationContext() , "Scan Complete" , Toast.LENGTH_LONG).show();
                    }
                }

                @Override
                public void onBatchScanResults(List<ScanResult> results) {
                    // Ignore for now
                }

                @Override
                public void onScanFailed(int errorCode) {
                    // Ignore for now
                }
            };

            listAdapter = new ScanDeviceAdapter(new ArrayList<BluetoothDevice>(), new ScanDeviceAdapter.OnItemClickListener() {
                @Override
                public void onClick(BluetoothDevice bluetoothDevice) {
                    scanner.stopScan(scanCallback);
                    Intent intent = new Intent(MainActivity.this , DeviceConnectedScreen.class);
                    intent.putExtra("BLE_DEVICE" , bluetoothDevice.getAddress());
                    startActivity(intent);
                }
            });
            recyclerView.setAdapter(listAdapter);

            scanner.startScan(null, new ScanSettings.Builder()
                    .setScanMode(ScanSettings.SCAN_MODE_LOW_POWER)
                    .setReportDelay(0L)
                    .build(), scanCallback);
            scanTime = System.currentTimeMillis();
            Log.d(TAG, "scan started");

        }else {
            Log.e(TAG, "could not get scanner object");
        }
    }

    /*@RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN_MR2)
    private void  writeCharacteristic(BluetoothGatt gatt , byte state){

        BluetoothGattService service = gatt.getService(UUID.fromString("00001523-1212-efde-1523-785feabcd123"));
        if(service!=null){
            BluetoothGattCharacteristic characteristic = service.getCharacteristic(UUID.fromString("00001525-1212-efde-1523-785feabcd123"));
            if(characteristic!=null){
                byte[] b = new byte[1];
                b[0] = state;
                characteristic.setValue(b);
                gatt.writeCharacteristic(characteristic);
            }else{
                Log.d(TAG , "Led chars not found!");
            }
        }else {
            Log.d(TAG , "LBS Service not found!");
        }

    }*/


    /*@RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN_MR2)
    private void  readCharacteristic(){
        BluetoothGattService service = bluetoothGatt.getService(UUID.fromString("00001523-1212-efde-1523-785feabcd123"));
        if(service!=null){
            BluetoothGattCharacteristic characteristic = service.getCharacteristic(UUID.fromString("00001525-1212-efde-1523-785feabcd123"));
            if(characteristic!=null){
                bluetoothGatt.readCharacteristic(characteristic);
            }else{
                Log.d(TAG , "Led chars not found!");
            }
        }else {
            Log.d(TAG , "LBS Service not found!");
        }

    }*/

    protected boolean containsDevice(BluetoothDevice device){

        for(BluetoothDevice dev : listAdapter.getDataset()){
            if(dev.getName().equals(device.getName())){
                return true;
            }
        }

        return false;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if(requestCode == 6969){
            if(resultCode == RESULT_CANCELED){
                AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
                builder.setCancelable(false).setTitle("Bluetooth").setMessage("App cannot work without Bluetooth!")
                        .setNegativeButton("OK", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                MainActivity.this.finish();
                            }
                        }).show();
            }else{
                continueOnCreate();
            }

        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        boolean flag = true;
        for(int result : grantResults){
            if(result == PackageManager.PERMISSION_DENIED){
                flag = false;
                break;
            }
        }

        if(flag){

            if(!adapter.isEnabled()){
                Intent reqBT = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(reqBT , 6969);
            }else{
                continueOnCreate();
            }
        }else{
            AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
            builder.setCancelable(false).setTitle("GPS").setMessage("BLE cannot scan without GPS Access.")
                    .setNegativeButton("OK", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            MainActivity.this.finish();
                        }
                    }).show();
        }

    }
}