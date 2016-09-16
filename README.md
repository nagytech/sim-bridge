# sim-bridge

This tiny application simulates vehicles crossing a bridge in a coordinated manner
all in real time.   Using separate threads per vehicle, several mutexes and condition variables
are used to ensure that the flow of traffic is both steady and safe.
