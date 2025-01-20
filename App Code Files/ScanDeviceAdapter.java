package com.puranish.bleblinkapp;

import android.bluetooth.BluetoothDevice;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.util.List;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

public class ScanDeviceAdapter extends RecyclerView.Adapter<ScanDeviceAdapter.ViewHolder> {


    private List<BluetoothDevice> mDataset;
    private OnItemClickListener listener;

    public ScanDeviceAdapter(List<BluetoothDevice> mDataset, OnItemClickListener listener) {
        this.mDataset = mDataset;
        this.listener = listener;
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View v = LayoutInflater.from(parent.getContext()).inflate(R.layout.item_scanned_device , parent , false);
        return new ViewHolder(v);
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
        final BluetoothDevice device = mDataset.get(position);
        holder.textView.setText(device.getName());
        holder.itemView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                listener.onClick(device);
            }
        });
    }

    @Override
    public int getItemCount() {
        return mDataset.size();
    }

    static class ViewHolder extends RecyclerView.ViewHolder{

        TextView textView;

        public ViewHolder(@NonNull View itemView) {
            super(itemView);
            textView = itemView.findViewById(R.id.device_name_tv);
        }
    }

    public List<BluetoothDevice> getDataset() {
        return mDataset;
    }

    public interface OnItemClickListener{
        void onClick(BluetoothDevice bluetoothDevice);
    }

}
