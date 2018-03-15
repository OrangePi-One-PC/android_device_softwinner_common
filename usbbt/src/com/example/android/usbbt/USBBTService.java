package com.example.android.usbbt;

import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.content.Context;
import android.os.IBinder;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.os.SystemProperties;

import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothAdapter;
import android.hardware.usb.UsbManager;
import android.hardware.usb.UsbDevice;

import com.softwinner.SystemMix;

import android.content.BroadcastReceiver;
import android.content.IntentFilter;

import java.io.InputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.HashMap;


/* This service start when boot complete and run all the time. */
public class USBBTService extends Service {
    private static final String TAG = "USBBTService";
    public static final String ACTION = "com.example.android.usbbt.USBBTService";
    private final String SERVER_NAME = "usb_bt_server";
    private static final String PROPERTY_NAME = "sys.usbbt.inserted";
    private static final String ACTION_USBBTDEV_CHANGE = "action.android.usbbt.change";

    private static final int MSG_DISABLE_BT = 1;
    private static final int MSG_INSERT_USB = 2;
    private static final int MSG_REMOVE_USB = 3;
    private static final int MSG_ENABLE_BT  = 4;

    private boolean should_enable_bt = false;

    private ServerSocketThread mServerSocketThread;

    private BluetoothManager mBluetoothManager;
    private BluetoothAdapter mBluetoothAdapter;


    @Override
    public void onCreate() {
        Log.v(TAG, "onCreate()...");
        super.onCreate();
    }

    @Override
    public void onDestroy() {
        Log.v(TAG, "onDestroy()...");
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.v(TAG, "onBind()...");
        return null;
    }

    @Override
    public void onStart(Intent intent, int startId) {
        Log.v(TAG, "onStart()...");

        mServerSocketThread = new ServerSocketThread();
        mServerSocketThread.start();

////////////////////////////////////////////////////////////////////////////
/* we check if usb dongle is inserted before boot complete. */
        UsbManager um = (UsbManager)getSystemService(Context.USB_SERVICE);
        if (um != null) {
            HashMap<String,UsbDevice> devlist = um.getDeviceList();
            int size = devlist.size();
            Log.d(TAG, " dev list size = " + size);
            if (devlist != null) {
                for (UsbDevice dev : devlist.values()) {
                    if ((dev.getDeviceClass() == 0xe0) && (dev.getDeviceSubclass() == 0x01) && (dev.getDeviceProtocol() == 0x01)) {
                        SystemMix.setProperty(PROPERTY_NAME, "1");
                        sendBroadcast(new Intent(ACTION_USBBTDEV_CHANGE));
                        Log.d(TAG, "USB Dongle is inserted before boot.");
                    }
                }
            } else {
                Log.e(TAG, "usb device list is null");
            }
        } else {
            Log.e(TAG, "UsbManager is null.");
        }
/////////////////////////////////end/////////////////////////////////////////

        super.onStart(intent, startId);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.v(TAG, "onStartCommand()...");
        return super.onStartCommand(intent, flags, startId);
    }

    private void enable_bt() {
        mBluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = mBluetoothManager.getAdapter();
        // Checks if Bluetooth is supported on the device.
        if (mBluetoothAdapter != null) {
            if (!mBluetoothAdapter.isEnabled())
                mBluetoothAdapter.enable();
        } else {
            Log.e(TAG, "BluetoothAdapter is NULL.");
        }
    }

    private void disable_bt() {
        mBluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = mBluetoothManager.getAdapter();
        // Checks if Bluetooth is supported on the device.
        if (mBluetoothAdapter != null) {
            if (mBluetoothAdapter.isEnabled()) {
                mBluetoothAdapter.disable();
                should_enable_bt = true;
            }
        } else {
            Log.e(TAG, "BluetoothAdapter is NULL.");
        }
    }

    private Handler mHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);

            if (SystemProperties.getInt("persist.service.bdroid.usbbt", 0) == 1) {
            switch (msg.what) {
                case MSG_DISABLE_BT: {
                    disable_bt();
                    break;
                }
                case MSG_INSERT_USB: {
                    SystemMix.setProperty(PROPERTY_NAME, "1");
                    sendBroadcast(new Intent(ACTION_USBBTDEV_CHANGE));
                    if (should_enable_bt) {
                        enable_bt();
                        should_enable_bt = false;
                    }
                    break;
                }
                case MSG_REMOVE_USB: {
                    SystemMix.setProperty(PROPERTY_NAME, "0");
                    sendBroadcast(new Intent(ACTION_USBBTDEV_CHANGE));
                    disable_bt();
                    break;
                }
                case MSG_ENABLE_BT: {
                    enable_bt();
                    break;
                }
                default:
                    break;
            }
            }
        }
    };


    private class ServerSocketThread extends Thread {
        private boolean keepRunning = true;
        private LocalServerSocket mLocalServerSocket;

        @Override
        public void run() {
            try {
                mLocalServerSocket = new LocalServerSocket(SERVER_NAME);
            }
            catch (IOException e) {
                e.printStackTrace();
                keepRunning = false;
            }

            Log.d(TAG, "waiting for new client!");

            while(keepRunning) {
            try {
                LocalSocket mLocalSocket = mLocalServerSocket.accept();

                if (mLocalSocket != null) {
                    if (keepRunning) {
                        Log.d(TAG, "new client coming!");

                        InputStream mInputStream = null;
                        try {
                            mInputStream = mLocalSocket.getInputStream();
                            InputStreamReader mInputStreamReader = new InputStreamReader(mInputStream);
                            char[] buf = new char[5];
                            int readBytes = -1;
                            readBytes = mInputStreamReader.read(buf);
                            Log.d(TAG, "read data: " + String.valueOf(buf[0]));
                            if ((readBytes == 1) && buf[0] == 1) {
                                Log.d(TAG, "start to disable bt...");
                                Message message = new Message();
                                message.what = MSG_DISABLE_BT;
                                mHandler.sendMessage(message);
                            } else if ((readBytes == 1) && buf[0] == 2) {
                                Log.d(TAG, "usb bt inserted...");
                                Message message = new Message();
                                message.what = MSG_INSERT_USB;
                                mHandler.sendMessage(message);
                            } else if ((readBytes == 1) && buf[0] == 3) {
                                Log.d(TAG, "usb bt removed...");
                                Message message = new Message();
                                message.what = MSG_REMOVE_USB;
                                mHandler.sendMessage(message);
                            }
                        }
                        catch (IOException e) {
                            e.printStackTrace();
                            Log.e(TAG, "read data error!");
                        }
                        finally {
                            if (mInputStream != null) {
                                try {
                                    mInputStream.close();
                                }
                                catch (IOException e) {
                                    e.printStackTrace();
                                }
                            }
                            if (mLocalSocket != null) {
                                try {
                                    mLocalSocket.close();
                                }
                                catch (IOException e) {
                                    e.printStackTrace();
                                }
                            }
                        }
                    }
                }
            }
            catch (IOException e) {
                e.printStackTrace();
                keepRunning = false;
            }
            }

            if (mLocalServerSocket != null) {
                try {
                    mLocalServerSocket.close();
                }
                catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

}
