def can_build(env, platform):
    return True

def configure(env):
    pass

def get_doc_classes():
    return [
        "GDProcMesh",
        "GDProcNode",
        "GDProcInCurve",
        "GDProcInInt",
        "GDProcInMesh",
        "GDProcInPoolVectors",
        "GDProcInReal",
        "GDProcInVector",
        "GDProcCount",
        "GDProcEuler",
        "GDProcRandom",
        "GDProcVector",
        "GDProcExec",
        "GDProcSplitVector",
        "GDProcAdd",
        "GDProcBevel",
        "GDProcDiv",
        "GDProcGenNormals",
        "GDProcMult",
        "GDProcRedist",
        "GDProcRotate",
        "GDProcRotMult",
        "GDProcScale",
        "GDProcSub",
        "GDProcTranslate",
        "GDProcBox",
        "GDProcCircle",
        "GDProcLine",
        "GDProcRect",
        "GDProcExtrudeShape",
        "GDProcSimplify",
        "GDProcSurface",
        "GDProcMerge",
        "GDProcMirror",
        "GDProcPlaceOnPath",
        "GDProcTransform",
        "GDProcOutput"
    ]

def get_doc_path():
    return "doc_classes"
