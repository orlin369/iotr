function DeviceStatus(url) {

    var url = url;

    var eventSource = new EventSource(url);

    function formatTwoDigits(i) {
        if (i < 10) { // add zero in front of numbers < 10
            i = '0' + i
        }
        return i;
    }

    function toPercentage(dbm) {
        // https://www.adriangranados.com/blog/dbm-to-percent-conversion
        // 100 x (1 - (PdBm_max - PdBm) / (PdBm_max - PdBm_min))
        return 100 * (1 - (0 - dbm)/(0 - -100));
    }

    this.init = function() {
        eventSource.onopen = function(e) {
            console.log("Device status: Opened");
        };
        eventSource.onerror = function(e) {
            if (e.target.readyState != EventSource.OPEN) {
                console.log("Device status: Closed");
            }
        };
        eventSource.onmessage = function(e) {
            console.log("Device status: " + e.data);
        };
        eventSource.addEventListener("deviceStatus", function(e) {
            if (!e.data)
            {
                return;
            }

            var status = JSON.parse(e.data);
            var unix_timestamp = status["ts"];
            // Create a new JavaScript Date object based on the timestamp
            // multiplied by 1000 so that the argument is in milliseconds, not seconds.
            var date = new Date(unix_timestamp * 1000);
            // Hours part from the timestamp
            var hours = formatTwoDigits(date.getHours());
            // Minutes part from the timestamp
            var minutes = formatTwoDigits(date.getMinutes());

            // Will display time in 10:30 format
            var formattedTime = hours + ':' + minutes;
            var clock = document.getElementById("h-clock");
            clock.innerHTML = formattedTime;

            var ssid = document.getElementById("h-ssid");
            ssid.innerHTML = status["ssid"];

            var rssi = document.getElementById("h-rssi");
            var percentage = toPercentage(status["rssi"]);
            rssi.innerHTML = percentage.toFixed(2);

            var voltage = document.getElementById("h-voltage");
            voltage.innerHTML = status["voltage"].toFixed(2) + "V";

            // TODO: Get it from flags.
            //var mqtt = document.getElementById("h-mqtt");
            //mqtt.innerHTML = status["mqtt_conn"];

        }, false);
    };
}