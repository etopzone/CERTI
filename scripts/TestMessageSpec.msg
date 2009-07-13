
// CERTI message description spec test file

enum MessageType {
   FIRST_VAL = 0,
   REAL_VAL1,
   REAL_VAL2,
   UNUSED
}

message MyMessage {}

// blah
native_message ANativeMessageType 

native_message ANotherativeMessageType 

message MyMessage { // another comment
    repeated int32 whatever
}

message MyOtherMessage : merge MyMessage {
	required bool constrained
}