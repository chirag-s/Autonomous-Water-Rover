File Structure

Arduino RC to PWM
--> Code to be uploaded on arduino used for reading Rc outputs from navio 2

Arduino Pro Mini
--> Code to be uploaded on arduino used for reading sensor data

SensorLog.py
--> Code to be executed on Raspberry pi having navio2 to get data from arduino having sensors

sensorsData.db
--> Sqlite3 database having two tables
----|GPS Data
-------> Table having only 1 row and is updated by GPS.py in folder WebServer. It stores the lat-lon values along with fix status
----|Sensor Data
-------> Table having sensor data and timestamp

WebServer
--|app.py
----> Execute to start web server
--|static
----> Contains all media, CSS and JS libraries
--|templates
----> Contains all HTML layouts
--| navio
----> Contains libraries necessary in app.py to send RC outputs
