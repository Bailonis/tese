package com.thesis.RoundedShoulders

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.bluetooth.BluetoothManager
import android.content.pm.PackageManager
import android.os.Bundle
import android.util.Log
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import java.io.IOException
import java.util.*

class MainActivity : AppCompatActivity() {

    private val MAC_ADDRESS_OF_ESP32 = "YOUR_ACTUAL_MAC_ADDRESS_HERE"
    private val REQUEST_BLUETOOTH_PERMISSION = 1001
    private lateinit var bluetoothAdapter: BluetoothAdapter
    private lateinit var bluetoothDevice: BluetoothDevice
    private lateinit var bluetoothSocket: BluetoothSocket
    private var isConnected = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val connectButton: Button = findViewById(R.id.connectButton)
        connectButton.setOnClickListener {
            checkBluetoothPermission()
        }
    }

    private fun checkBluetoothPermission() {
        if (ContextCompat.checkSelfPermission(
                this,
                Manifest.permission.BLUETOOTH_ADMIN
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            ActivityCompat.requestPermissions(
                this,
                arrayOf(Manifest.permission.BLUETOOTH_ADMIN),
                REQUEST_BLUETOOTH_PERMISSION
            )
        } else {
            setupBluetoothConnection()
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<String>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        when (requestCode) {
            REQUEST_BLUETOOTH_PERMISSION -> {
                if ((grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED)) {
                    setupBluetoothConnection()
                } else {
                    // Permission denied. Handle accordingly, e.g. show a dialog or Toast informing the user.
                }
                return
            }
        }
    }

    private fun setupBluetoothConnection() {
        if (ContextCompat.checkSelfPermission(
                this,
                Manifest.permission.BLUETOOTH_ADMIN
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            ActivityCompat.requestPermissions(
                this,
                arrayOf(Manifest.permission.BLUETOOTH_ADMIN),
                REQUEST_BLUETOOTH_PERMISSION
            )
        } else {
            establishBluetoothConnection()
        }
    }

    private fun establishBluetoothConnection() {
        try {
            val bluetoothManager = getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
            bluetoothAdapter = bluetoothManager.adapter

            bluetoothDevice = bluetoothAdapter.getRemoteDevice(MAC_ADDRESS_OF_ESP32)

            Thread {
                try {
                    bluetoothSocket = bluetoothDevice.createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"))
                    bluetoothSocket.connect()
                    isConnected = true

                    val inputStream = bluetoothSocket.inputStream

                    while (isConnected) {
                        val data = inputStream.read()
                        Log.d("MainActivity", "Received data from ESP32: $data")
                    }
                } catch (e: IOException) {
                    e.printStackTrace()
                    isConnected = false
                }
            }.start()
        } catch (se: SecurityException) {
            // Handle the security exception appropriately.
            Log.e("MainActivity", "Security Exception: ${se.message}")
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        try {
            isConnected = false
            bluetoothSocket.close()
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }
}