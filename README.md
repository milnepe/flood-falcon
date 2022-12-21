# Flood Falcon
An Arduino powered flood warning device

## Introduction
The Environment Agency flood-monitoring API provides developers with access to near real time information covering:

flood warnings and flood alerts
flood areas which to which warnings or alerts apply
measurements of water levels and flows
information on the monitoring stations providing those measurements
Water levels and flows are regularly monitored, usually every 15 minutes. However, data is transferred back to the Environment Agency at various frequencies, usually depending on the site and level of flood risk. Transfer of data is typically once or twice per day, but usually increases during times of heightened flood risk.

These APIs are provided as open data under the Open Government Licence with no requirement for registration. If you make use of this data please acknowledge this with the following attribution statement:

this uses Environment Agency flood and river level data from the real-time data API (Beta)

## Getting your flood warning area
This easiest way to find out your flood area but there needs to be an alert or warning in place to do this:

You can get all the flood warnings in the UK in you browser by visiting the following URL:
https://check-for-flooding.service.gov.uk/alerts-and-warnings

Click on the warning you are interested in for example the Upper Ouse which is:
https://check-for-flooding.service.gov.uk/target-area/065WAF441

The last part of the URL above is the flood warning area code in this example it is 065WAF441

## Testing
You can test the API for your flood area by making a request in your browser to the following URI:
http://environment.data.gov.uk/flood-monitoring/id/floods/{your-flood-area-code}

So in the example above for the Upper Ouse (code 065WAF441) use this URI:
http://environment.data.gov.uk/flood-monitoring/id/floodAreas/065WAF441

This will return a JSON object with all the flood information for your area.

The API is documented here:
http://environment.data.gov.uk/flood-monitoring/doc/reference#flood-warnings

## Falcon display
The Falcon polls the API at the frequency you set and it receives the API response and displays the following data:

An icon representing the flood warning which may be at one of four possible severity levels - derived from the severityLevel:

Level	Name				Meaning
1	Severe Flood Warning		Severe Flooding, Danger to Life.
2	Flood Warning			Flooding is Expected, Immediate Action Required.
3	Flood Alert			Flooding is Possible, Be Prepared.
4	Warning no Longer in Force	The warning is no longer in force

The severity level description - derived from severity

The update date / time - derived from timeRaised (The date and time the warning was last reviewed. Usually (but not always) this leads to a change in the message or severity)

## Configuration
User config is in "falcon_config.h"

You need to add your WiFi details and setup your server limits. Also the time interval can be adjusted.

Audio clips are in ./audio - connect your sound board to your PC and copy the clips to the mounted drive.

## Buttons
RH button is dual mode - short press to replay alert - long press to toggle audio off / on
LH button - press for demo mode - this cycles through the different states - press reset to exit demo mode
External demo button - same as above
Middle button - reset
