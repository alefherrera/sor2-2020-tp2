## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    obj = bld.create_ns3_program('tp-redes', ['csma', 'internet', 'applications'])
    obj.source = 'tp-redes.cc'
