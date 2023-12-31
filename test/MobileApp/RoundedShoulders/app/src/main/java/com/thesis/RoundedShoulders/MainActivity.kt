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
import android.widget.ArrayAdapter
import android.widget.Button
import android.widget.ListView
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import java.io.IOException
import java.util.*

class MainActivity : AppCompatActivity() {

    private val MAC_ADDRESS_OF_ESP32 = "7C:9E:BD:4B:B0:7E"
    private val REQUEST_BLUETOOTH_PERMISSION = 1001
    private lateinit var bluetoothAdapter: BluetoothAdapter
    private lateinit var bluetoothDevice: BluetoothDevice
    private lateinit var bluetoothSocket: BluetoothSocket
    private var isConnected = false
    private lateinit var statusTextView: TextView
    private lateinit var listView: ListView
    private val dataList: MutableList<String> = mutableListOf()
    private lateinit var adapter: ArrayAdapter<String>



    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val connectButton: Button = findViewById(R.id.connectButton)
        statusTextView = findViewById(R.id.statusTextView)
        listView = findViewById(R.id.listView)
        adapter = ArrayAdapter(this, android.R.layout.simple_list_item_1, dataList)
        listView.adapter = adapter



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

                    runOnUiThread {
                        statusTextView.text = getString(R.string.status_connected)
                    }

                    val inputStream = bluetoothSocket.inputStream
                    val outputStream = bluetoothSocket.outputStream

                    val receivedString = StringBuilder()

                    while (isConnected) {
                        val byteReceived = inputStream.read()

                        if (byteReceived != -1) {  // Check if the end of the stream has not been reached
                            val readChar = byteReceived.toChar()

                            // Check if the received character is a newline character
                            if (readChar == '\n') {
                                val completeString = receivedString.toString()
                                runOnUiThread {
                                    // Add the received message to the dataList and notify the adapter
                                    dataList.add(completeString)
                                    adapter.notifyDataSetChanged()
                                }
                                receivedString.clear()  // Clear the StringBuilder for the next message
                            } else {
                                receivedString.append(readChar)
                            }

                            outputStream.write(byteReceived)
                        }
                    }

                } catch (e: IOException) {
                    e.printStackTrace()
                    isConnected = false
                    runOnUiThread {
                        statusTextView.text = getString(R.string.status_connection_failed)
                    }
                }
            }.start()
        } catch (se: SecurityException) {
            // Handle the security exception appropriately.
            Log.e("MainActivity", "Security Exception: ${se.message}")
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        disconnectBluetooth()
    }

    private fun disconnectBluetooth() {
        isConnected = false
        try {
            bluetoothSocket.close()
        } catch (e: IOException) {
            e.printStackTrace()
        }
        statusTextView.text = getString(R.string.status_not_connected)
    }
}
