
//Starts the logging if a StatusMessage with a Temperature value of over 100 is received
on CanMessage<*> StatusMessage {
  if(this.Temperature.Phys > 100 && loggerStatus() == LOGGER_STATE_IDLE){
    loggerStart();
  }
}

//Stops logging if a message with id 1 is received
on prefilter CanMessage<*> 1 {
  if(loggerStatus() != LOGGER_STATE_IDLE){
    filterDropMessage();
    loggerStop();
  }
}