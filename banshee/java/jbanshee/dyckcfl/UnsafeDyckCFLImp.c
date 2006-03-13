/*
 * Copyright (c) 2000-2004
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
#include <jni.h>
#include "jbanshee_dyckcfl_UnsafeDyckCFL.h"
#include "dyckcfl.h"
#include "nonspec.h"

union dn {
  dyck_node node;
  jlong node_id;
};

static dyck_node node_id_to_node(jlong nodeID) {
  return ((union dn)nodeID).node;
}

static jlong node_to_node_id(dyck_node n) {
  return ((union dn)n).node_id;
}

JNIEXPORT void JNICALL Java_jbanshee_dyckcfl_UnsafeDyckCFL_initialize
(JNIEnv *env, jclass clazz) {
  nonspec_init();
  dyck_init(TRUE);
}

JNIEXPORT void JNICALL Java_jbanshee_dyckcfl_UnsafeDyckCFL_printDyckConstraints
  (JNIEnv *env, jobject obj, jboolean enablePrint) {
  if (enablePrint) flag_dyck_print_constraints = TRUE;
  else flag_dyck_print_constraints = FALSE;
}

JNIEXPORT jlong JNICALL Java_jbanshee_dyckcfl_UnsafeDyckCFL_makeTaggedNode
  (JNIEnv *env, jobject obj, jstring name) {

  const char *str = (*env)->GetStringUTFChars(env, name, 0);
  dyck_node result = make_tagged_dyck_node(str);
  (*env)->ReleaseStringUTFChars(env, name, str);

  return node_to_node_id(result);
}

JNIEXPORT jlong JNICALL Java_jbanshee_dyckcfl_UnsafeDyckCFL_makeUntaggedNode
  (JNIEnv *env, jobject obj, jstring name) { 

  const char *str = (*env)->GetStringUTFChars(env, name, 0);
  dyck_node result = make_untagged_dyck_node(str);
  (*env)->ReleaseStringUTFChars(env, name, str);

  return node_to_node_id(result);
}

JNIEXPORT void JNICALL Java_jbanshee_dyckcfl_UnsafeDyckCFL_markNodeGlobal
  (JNIEnv *env, jobject obj, jlong nodeID) { 

  dyck_node n = node_id_to_node(nodeID);
  mark_dyck_node_global(n);
}

JNIEXPORT void JNICALL Java_jbanshee_dyckcfl_UnsafeDyckCFL_makeSubtypeEdge
  (JNIEnv *env, jobject obj, jlong nodeID1, jlong nodeID2) { 

  dyck_node n1, n2;

  n1 = node_id_to_node(nodeID1);
  n2 = node_id_to_node(nodeID2);

  make_dyck_subtype_edge(n1, n2);
}

JNIEXPORT void JNICALL Java_jbanshee_dyckcfl_UnsafeDyckCFL_makeOpenEdge
  (JNIEnv *env, jobject obj, jlong nodeID1, jlong nodeID2 , jint index) { 

  dyck_node n1, n2;
  n1 = node_id_to_node(nodeID1);
  n2 = node_id_to_node(nodeID2);
  make_dyck_open_edge(n1, n2, index);
}

JNIEXPORT void JNICALL Java_jbanshee_dyckcfl_UnsafeDyckCFL_makeCloseEdge
  (JNIEnv *env, jobject obj, jlong nodeID1, jlong nodeID2, jint index) { 

  dyck_node n1, n2;
  n1 = node_id_to_node(nodeID1);
  n2 = node_id_to_node(nodeID2);
  make_dyck_close_edge(n1, n2, index);

}

JNIEXPORT void JNICALL Java_jbanshee_dyckcfl_UnsafeDyckCFL_makeContraOpenEdge
  (JNIEnv *env, jobject obj, jlong nodeID1, jlong nodeID2 , jint index) { 

  dyck_node n1, n2;
  n1 = node_id_to_node(nodeID1);
  n2 = node_id_to_node(nodeID2);
  make_dyck_contra_open_edge(n1, n2, index);
}

JNIEXPORT void JNICALL Java_jbanshee_dyckcfl_UnsafeDyckCFL_makeContraCloseEdge
  (JNIEnv *env, jobject obj, jlong nodeID1, jlong nodeID2, jint index) { 

  dyck_node n1, n2;
  n1 = node_id_to_node(nodeID1);
  n2 = node_id_to_node(nodeID2);
  make_dyck_contra_close_edge(n1, n2, index);

}



JNIEXPORT jboolean JNICALL Java_jbanshee_dyckcfl_UnsafeDyckCFL_checkReaches
  (JNIEnv *env, jobject obj, jlong nodeID1, jlong nodeID2) { 
  
  dyck_node n1, n2;

  n1 = node_id_to_node(nodeID1);
  n2 = node_id_to_node(nodeID2);

  return dyck_check_reaches(n1,n2);
}

JNIEXPORT jboolean JNICALL Java_jbanshee_dyckcfl_UnsafeDyckCFL_checkPNReaches
  (JNIEnv *env, jobject obj, jlong nodeID1, jlong nodeID2) { 

  dyck_node n1, n2;

  n1 = node_id_to_node(nodeID1);
  n2 = node_id_to_node(nodeID2);

  return dyck_check_pn_reaches(n1,n2);
}


