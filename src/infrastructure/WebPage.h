#pragma once
#include <Arduino.h>

namespace Blinds {
    namespace Infrastructure {

        const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Blind Controller</title>
    <script src="https://cdn.tailwindcss.com"></script>
    <style>
        /* Custom slider styling */
        input[type=range]::-webkit-slider-thumb {
            -webkit-appearance: none;
            height: 24px;
            width: 24px;
            border-radius: 50%;
            background: #4F46E5;
            cursor: pointer;
            margin-top: -10px;
        }
        input[type=range]::-webkit-slider-runnable-track {
            width: 100%;
            height: 4px;
            cursor: pointer;
            background: #E5E7EB;
            border-radius: 2px;
        }
    </style>
</head>
<body class="bg-gray-100 min-h-screen flex items-center justify-center p-4">
    <div class="bg-white rounded-xl shadow-lg p-8 w-full max-w-md">
        <div class="flex justify-between items-center mb-6">
            <h1 class="text-2xl font-bold text-gray-800">Smart Blinds</h1>
            <div id="connectionStatus" class="w-3 h-3 rounded-full bg-red-500"></div>
        </div>

        <!-- Status Display -->
        <div class="bg-gray-50 rounded-lg p-4 mb-6 text-center">
            <div class="text-gray-500 text-sm font-medium uppercase tracking-wide">Status</div>
            <div id="statusText" class="text-xl font-bold text-gray-900 mt-1">CONNECTING...</div>
            <div class="text-3xl font-bold text-indigo-600 mt-2" id="positionText">--%</div>
        </div>

        <!-- Position Slider -->
        <div class="mb-8">
            <div class="flex justify-between text-xs text-gray-500 mb-2">
                <span>OPEN (0%)</span>
                <span>CLOSED (100%)</span>
            </div>
            <input type="range" id="positionSlider" min="0" max="100" value="50" class="w-full h-2 bg-gray-200 rounded-lg appearance-none cursor-pointer" onchange="setPosition(this.value)">
        </div>

        <!-- Control Buttons -->
        <div class="grid grid-cols-3 gap-4 mb-6">
            <button onclick="sendCommand('up')" class="bg-gray-100 hover:bg-gray-200 text-gray-800 font-bold py-4 px-4 rounded-lg flex flex-col items-center transition duration-200">
                <svg xmlns="http://www.w3.org/2000/svg" class="h-8 w-8 mb-1" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                    <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M5 15l7-7 7 7" />
                </svg>
                UP
            </button>
            <button onclick="sendCommand('stop')" class="bg-red-50 hover:bg-red-100 text-red-600 font-bold py-4 px-4 rounded-lg flex flex-col items-center transition duration-200">
                <svg xmlns="http://www.w3.org/2000/svg" class="h-8 w-8 mb-1" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                    <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M6 18L18 6M6 6l12 12" />
                </svg>
                STOP
            </button>
            <button onclick="sendCommand('down')" class="bg-gray-100 hover:bg-gray-200 text-gray-800 font-bold py-4 px-4 rounded-lg flex flex-col items-center transition duration-200">
                <svg xmlns="http://www.w3.org/2000/svg" class="h-8 w-8 mb-1" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                    <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M19 9l-7 7-7-7" />
                </svg>
                DOWN
            </button>
        </div>

        <!-- Extra Controls -->
        <div class="border-t border-gray-200 pt-6">
            <button onclick="sendHome()" class="w-full text-gray-500 hover:text-indigo-600 text-sm font-medium transition duration-200 flex items-center justify-center gap-2">
                <svg xmlns="http://www.w3.org/2000/svg" class="h-4 w-4" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                    <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 4v5h.582m15.356 2A8.001 8.001 0 004.582 9m0 0H9m11 11v-5h-.581m0 0a8.003 8.003 0 01-15.357-2m15.357 2H15" />
                </svg>
                Recalibrate / Home
            </button>
        </div>
    </div>

    <script>
        const API_URL = ''; // Relative

        function updateStatus() {
            fetch(API_URL + '/state')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('connectionStatus').classList.remove('bg-red-500');
                    document.getElementById('connectionStatus').classList.add('bg-green-500');

                    document.getElementById('statusText').innerText = data.state;
                    document.getElementById('positionText').innerText = data.position + '%';

                    // Only update slider if not dragging?
                    // For simplicity, just update it.
                    const slider = document.getElementById('positionSlider');
                    if (document.activeElement !== slider) {
                        slider.value = data.position;
                    }
                })
                .catch(err => {
                    document.getElementById('connectionStatus').classList.remove('bg-green-500');
                    document.getElementById('connectionStatus').classList.add('bg-red-500');
                });
        }

        function sendCommand(cmd) {
            fetch(API_URL + '/move', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({cmd: cmd})
            }).then(updateStatus);
        }

        function setPosition(percent) {
            fetch(API_URL + '/goto', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({percent: parseInt(percent)})
            }).then(updateStatus);
        }

        function sendHome() {
            if(confirm('This will move the blind to find limits. Ensure path is clear.')) {
                fetch(API_URL + '/home', { method: 'POST' }).then(updateStatus);
            }
        }

        // Poll every 1 second
        setInterval(updateStatus, 1000);
        updateStatus();
    </script>
</body>
</html>
)rawliteral";
    }
}
