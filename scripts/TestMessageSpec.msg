// CERTI message description spec test file
package my.hierarchy.pack // don't you know it

//
//  This is a multiline
//  comment
//
enum MessageType {
   FIRST_VAL = 0, // one may specify the value of the first enum symbol
   REAL_VAL1    ,
   REAL_VAL2    ,
   UNUSED
}

message MyMessage {} 

// blah
native_message ANativeMessageType 

native_message ANotherativeMessageType 

message MyMessage { 
    repeated int32 whatever
}

message MyOtherMessage : merge MyMessage {
    required bool              constrained	
	required MessageType       type
	required uint32            a
}