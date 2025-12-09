#ifndef GEMPP_PROGRAM_H
#define GEMPP_PROGRAM_H

// Minimal stub for Qt-free build
class DLL_EXPORT Program {
    public:
        enum Sense {
            MINIMIZE = 0,
            MAXIMIZE
        };

        Program() {}
        virtual ~Program() {}
};

#endif /* GEMPP_PROGRAM_H */