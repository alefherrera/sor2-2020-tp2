## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    obj = bld.create_ns3_program('tp-redes', ['csma', 'point-to-point', 'point-to-point-layout','internet', 'applications', 'netanim'])
    obj.source = 'tp-redes.cc'
