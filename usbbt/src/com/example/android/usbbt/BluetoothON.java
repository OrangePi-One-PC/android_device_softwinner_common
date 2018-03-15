package com.example.android.usbbt;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.bluetooth.BluetoothAdapter;


public class BluetoothON extends BroadcastReceiver {
    @Override 
    public void onReceive(Context context, Intent intent) {

        if (intent.getAction().equals(BluetoothAdapter.ACTION_STATE_CHANGED))
        {
            Log.d("USBBT", "Bluetooth state change...");

            if (intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, 0) == BluetoothAdapter.STATE_ON)
                context.startService(new Intent(USBBTService.ACTION));
            else if (intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, 0) == BluetoothAdapter.STATE_OFF)
                context.stopService(new Intent(USBBTService.ACTION));
        }
    }
}
