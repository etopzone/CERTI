//
// CERTI message description spec test file
// 
package my.hierarchy.pack

//
//  This is a multiline
//  comment
//

enum EType {
   FIRST_VAL = 0, // one may specify the value of the first enum symbol
   REAL_VAL1    ,
   REAL_VAL2    ,
   UNUSED
}

message MyMessage {} 

// blah
native_message ANativeMessageType 

native_message ANotherativeMessageType 

message MyMessage2 { 
    repeated int32 whatever
}

message MyOtherMessage : merge MyMessage {
    required bool              constrained  // constrain 
	required MessageType       type
	required uint32            a [ default=1 ]
}