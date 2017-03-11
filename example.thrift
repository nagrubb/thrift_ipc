service Example {
  void ping()
  bool registerHandler(1:i16 eventPort)
  oneway void wait()
}

service EventHandler {
  oneway void event(1:i32 sequence)
}
