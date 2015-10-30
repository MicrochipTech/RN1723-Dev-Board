<!DOCTYPE html>
<html lang="en">
    <head>
        %include header

        <script type="text/javascript">
            var httpReq;

            function initializePage() {
                // Initalize tiltvibe checkbox element
                if('{{eclient.system['tiltVibe']}}' == 'True')
                    document.getElementById("tiltVibe").checked = true;
                else
                    document.getElementById("tiltVibe").checked = false;

                // Initalize temp Scale element and display temp alarm for that scale
                var tempF = '{{eclient.system['temperatureThreshold']}}';

                // Initalize temp Scale element and display temp alarm for that scale
                if('{{eclient.system['tempScale']}}' == 'fahrenheit')
                {
                    document.getElementById('fahrenheit').checked = true;
                    document.getElementById('celsius').checked = false;
                    document.getElementById("tempSetting").innerHTML = "Alarm Setting: " + tempF + "&deg F"

                }
                else
                {
                    var curVal = document.getElementById("tempAlarm").value;
                    document.getElementById('fahrenheit').checked = false;
                    document.getElementById('celsius').checked = true;
                    tempC = ((tempF - 32) * 5/9);
                    document.getElementById("tempSetting").innerHTML = "Alarm Setting: " + tempC.toFixed(0) + "&deg C"
                    document.getElementById("tempAlarm").value = Math.round(((curVal - 32) * 5/9));
                }

            }

            function changeTempScale(scale) {
                var curVal = document.getElementById("tempAlarm").value;
                if(scale == 'celsius')
                    document.getElementById("tempAlarm").value = Math.round(((curVal - 32) * 5/9));
                else
                    document.getElementById("tempAlarm").value = Math.round(((curVal * 9/5) + 32));
            }

            function loadJSONDoc(url, cfunc, data)
            {
                // Create and send the request for appropriate browser
                if (window.XMLHttpRequest)
                {   // code for IE7+, Firefox, Chrome, Opera, Safari
                    httpReq = new XMLHttpRequest();
                    httpReq.open((data==null)?"GET":"POST", url, true);
                    httpReq.send(data);
                }
                else
                {   // code for IE6, IE5
                    httpReq = new ActiveXObject("Microsoft.XMLHTTP");
                    httpReq.open((data==null)?"GET":"POST", url, true);
                    httpReq.send(data);
                }
                httpReq.onreadystatechange = cfunc;
            }

            function GetItems()
            {
                loadJSONDoc('/RN1723/App1/v1/{{eclient.uuid}}/updatestatus.json',function()
                {
                    if (httpReq.readyState==4 && httpReq.status==200)
                    {
                        var jsonobj = JSON.parse(httpReq.responseText); 


                        switch_val = jsonobj.status.switches;

                        var whichButton = document.getElementById("button1");

                        if ((switch_val & 0x01) != 0)
                        {
                            whichButton.style.backgroundColor = "transparent";
                            whichButton.style.border = "1px solid #C8C8C8";
                        }
                        else
                        {
                            whichButton.style.backgroundColor = "#ff0000";
                            whichButton.style.border = "none";
                        }

                        var whichButton = document.getElementById("button2");
                        if ((switch_val & 0x02) != 0)
                        {
                            whichButton.style.backgroundColor = "transparent";
                            whichButton.style.border = "1px solid #C8C8C8";
                        }
                        else
                        {
                            whichButton.style.backgroundColor = "#ff0000";
                            whichButton.style.border = "none";
                        }

                        // Determine Temperature Scale
                        if('{{eclient.system['tempScale']}}' == 'fahrenheit')
                        {
                            document.getElementById("tempValue").innerHTML = jsonobj.status.temperature + "&deg F";
                            document.getElementById("tempSetting").innerHTML = "Alarm Setting: " + jsonobj.system.temperatureThreshold + "&deg F"
                        }
                        else
                        {
                            var degCel = ((jsonobj.status.temperature - 32) * 5/9); 
                            var setTempC = ((jsonobj.system.temperatureThreshold - 32) * 5/9); 
                            document.getElementById("tempValue").innerHTML = degCel.toFixed(0)  + "&deg C";
                            document.getElementById("tempSetting").innerHTML = "Alarm Setting: " + setTempC.toFixed(0) + "&deg C"
                        }

                        document.getElementById("potValue").innerHTML = jsonobj.status.potvalue;

                        alarm = jsonobj.status.alarms;

                        if (alarm & 0x01)
                        {
                            document.getElementById("tempValue").className = "text-danger";
                            document.getElementById("temp-panel").className = "panel panel-danger";
                        }
                        else
                        {
                            document.getElementById("tempValue").className = "text-success";
                            document.getElementById("temp-panel").className = "panel panel-default";
                        }
                        
                        if (alarm & 0x02)
                        {    
                            document.getElementById("potValue").className = "text-danger";
                            document.getElementById("pot-panel").className = "panel panel-danger";
                        }
                        else
                        {
                            document.getElementById("potValue").className = "text-success";
                            document.getElementById("pot-panel").className = "panel panel-default";
                        }

                        setTimeout(GetItems(), 2000);

                    }
                });
            }

            httpReq.onload = GetItems();


        </script>

    </head>
    <body onload="initializePage()">
        <!-- Include each module that makes up each page -->
        %include navbar

        <div class="container-fluid">
            <div class="row" id="header_image">
                <div class="col-sm-10 col-sm-offset-1 col-lg-8 col-lg-offset-2">
                    <img src="static/header_logo.png" class="img-responsive" alt="RN1723 Development Kit Server">
                </div>
            </div>
        </div>
        <!-- Load the rest of the page contents based on status of cookie -->

        <div class="container-fluid">
            <div class="row">
                <div class="col-sm-10 col-sm-offset-1 col-lg-8 col-lg-offset-2">
                    <div class="panel panel-default">
                        <div class="panel-heading">
                            <p class="panel-title">Device: {{eclient.uuid}}</a></p>
                        </div>
                        <div class="panel-body">

                            <div class="row">
                                <div class="col-xs-6 text-center">
                                    <div id="button1" class="button_icon"></div>
                                    <div>
                                        <h4>SW0</h4>
                                    </div>
                                </div>

                                <div class="col-xs-6 text-center">
                                    <div id="button2" class="button_icon"></div>
                                    <div>
                                        <h4>SW1</h4>
                                    </div>
                                </div>
                            </div>

                            <div class="row">
                                <div class="col-xs-6 text-center">
                                    <div id="temp-panel" class="panel panel-default">  
                                        <div class="panel-heading">
                                            <p class="panel-title">Temperature</p>
                                        </div>
                                        <div class="panel-body">
                                            <h1 class="text-success" id="tempValue"></h1>
                                        </div>
                                        <div class="panel-footer clearfix">
                                            <p id="tempSetting"></p>
                                        </div>
                                    </div>
                                </div>

                                <div class="col-xs-6 text-center">
                                    <div id="pot-panel" class="panel panel-default">  
                                        <div class="panel-heading">
                                            <p class="panel-title">Potentiometer</p>
                                        </div>
                                        <div class="panel-body">
                                            <h1 class="text-success" id="potValue"></h1>
                                        </div>
                                        <div class="panel-footer clearfix">
                                            <p id="potSetting">Alarm Setting: {{eclient.system['potMaxThreshold']}}</p>
                                        </div>
                                    </div>
                                </div>
                            </div>

                        </div>
                    </div>
                </div>
            </div>
        </div>

         <div class="container-fluid">
            <div class="row">
                <div class="col-sm-10 col-sm-offset-1 col-lg-8 col-lg-offset-2">
                    <div class="panel panel-default">
                        <div class="panel-heading">
                            <p class="panel-title">Device Settings</p>
                        </div>
                        <div class="panel-body">
                            <form role="form" action="change_settings" method="post">
                                <div class="form-group">
                                    <label for="deviceMAC">Post Interval(seconds):</label>
                                    <input type="number" class="form-control" name="postInterval" min="1" max="120" value="{{eclient.system['publishPeriod']}}">
                                </div>
                                <div class="form-group">
                                    <label for="pwd">Temp Alarm Settings:</label>
                                    <input type="number" id="tempAlarm" class="form-control" name="max_temp" min="-10" max="110" value="{{eclient.system['temperatureThreshold']}}">
                                </div>
                                <div class="form-group">
                                    <label for="pwd">POT Alarm Settings (between 0 and 1024):</label>
                                    <input type="number" class="form-control" name="max_pot" min="0" max="1024" value="{{eclient.system['potMaxThreshold']}}">
                                </div>
                                <div class="form-group">
                                    <label><input type="checkbox" id="tiltVibe" name="tiltVibe" value="True">Enable Tilt/Vibe Sensor</label><br/>
                                </div>
                                <div class="form-group">
                                    <label>Temperature Scale:</label><br>
                                    <input type="radio" id="fahrenheit" name="tempScale" value="fahrenheit" onclick="changeTempScale('fahrenheit')" checked> Fahrenheit<br>
                                    <input type="radio" id="celsius" name="tempScale" value="celsius" onclick="changeTempScale('celsius')"> Celsius<br>
                                </div>
                                <button type="submit" class="btn btn-default">Submit</button>
                            </form>
                        </div>
                    </div>
                </div>
            </div>
        </div>

        <!-- Include the javascript for the page -->
        <script src="static/jquery-1.11.1.min.js"></script>
        <script src="static/bootstrap.min.js"></script>


    </body>

    <footer>
        %include footer
    </footer>
</html>
