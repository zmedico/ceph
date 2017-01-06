// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2006 Sage Weil <sage@newdream.net>
 * Portions Copyright (C) 2013 CohortFS, LLC
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

#ifndef CLIENT_MESSAGE_FACTORY_H
#define CLIENT_MESSAGE_FACTORY_H
#include "msg/Message.h"
#include "msg/MessageFactory.h"

class CephContext;

class LibradosMessageFactory : public MessageFactory {
 private:
  CephContext *cct;
  MessageFactory *parent;
 public:
  LibradosMessageFactory(CephContext *cct, MessageFactory *parent=nullptr)
    : cct(cct), parent(parent) {}

  Message* create(int type);
};

#endif // CLIENT_MESSAGE_FACTORY_H
