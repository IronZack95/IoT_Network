# IoT_Network
 Personal Internet of Things Network


 \newpage
 \section{Introduction}
The goal of this project is to develop a sensor system, on hardware, with IoT technologies for monitoring various environmental parameters in a small cellar where we produce and store wine for family use every year. The main parameters that I want to observe are humidity, temperature and percentage saturation of the CO2 level in the air, which is the gas that can most cause asphyxiation problems during the fermentation phase of wine in a closed place.
In this project I want both to seek efficient and economical solutions but also to be complementary to the current state of my home computer network. The long-term objectives of this project are not limited to the collection of raw data of environmental parameters, but would like to make the whole system automatic and able to act on the environment through specific actuators autonomously, such as turning on the automatic ventilation system, heating or dehumidification of the cellar.
 \subsection{Network Map}
As you can see in the figure, the physical structure of the network is divided between two wifi routers connected in LAN, 20m away from each other, one of which is the main router connected to the internet and the other works as an Access Point. The sensors in the cellar are connected via wifi to the main router, about 10m away in a straight line. The sensors and actuators communicate over the network using the MQTT protocol to my Homelab server, which will manage the logic and communications of the entire local network.


 % \newpage
 \section{Server Side}

 \subsection{Architecture}

 \subsection{Node-Red}


 % \newpage
 \section{Mote Side}

 \subsection{ESP8266}

 \subsection{Environment Board}

 \subsection{Relay}

 \end{document}
