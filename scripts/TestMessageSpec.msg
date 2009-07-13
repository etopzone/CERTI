
// CERTI message description spec test file

message MyMessage {}

// blah
native_message ANativeMessageType 

native_message ANotherativeMessageType 

message MyMessage { // another comment
}

message MyOtherMessage : merge MyMessage {
	required bool constrained [default=false]
}