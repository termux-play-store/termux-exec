#include <termux/termux_exec__c/v1/termux/api/termux_exec/exec/ExecIntercept.h>



void test__inspectFileHeader();
void test__modifyExecEnv();



void ExecIntercept_runTests() {
    logVerbose(LOG_TAG, "ExecIntercept_runTests()");

    test__inspectFileHeader();
    test__modifyExecEnv();

    int__AEqual(0, errno);
}





void test__inspectFileHeader__Basic();

void test__inspectFileHeader() {
    logVerbose(LOG_TAG, "test__inspectFileHeader()");

    test__inspectFileHeader__Basic();

    int__AEqual(0, errno);
}

void test__inspectFileHeader__Basic() {
    logVVerbose(LOG_TAG, "test__inspectFileHeader__Basic()");

    char header[FILE_HEADER__BUFFER_LEN];
    size_t hsize = sizeof(header);

    struct FileHeaderInfo info = {.interpreterArg = NULL};

    snprintf(header, hsize, "#!/bin/sh\n") < 0 ? abort() : (void)0;
    inspectFileHeader(TERMUX__PREFIX, header, sizeof(header) , &info);
    state__ATrue(!info.isElf);
    state__ATrue(!info.isNonNativeElf);
    string__AEqual(TERMUX__PREFIX__BIN_DIR "/sh", info.interpreterPath);
    state__ATrue(info.interpreterArg == NULL);

    snprintf(header, hsize, "#!/bin/sh -x\n") < 0 ? abort() : (void)0;
    inspectFileHeader(TERMUX__PREFIX, header, sizeof(header) , &info);
    state__ATrue(!info.isElf);
    state__ATrue(!info.isNonNativeElf);
    string__AEqual(TERMUX__PREFIX__BIN_DIR "/sh", info.interpreterPath);
    string__AEqual("-x", info.interpreterArg);

    snprintf(header, hsize, "#! /bin/sh -x\n") < 0 ? abort() : (void)0;
    inspectFileHeader(TERMUX__PREFIX, header, sizeof(header) , &info);
    state__ATrue(!info.isElf);
    state__ATrue(!info.isNonNativeElf);
    string__AEqual(TERMUX__PREFIX__BIN_DIR "/sh", info.interpreterPath);
    string__AEqual("-x", info.interpreterArg);

    snprintf(header, hsize, "#!/bin/sh -x \n") < 0 ? abort() : (void)0;
    inspectFileHeader(TERMUX__PREFIX, header, sizeof(header) , &info);
    state__ATrue(!info.isElf);
    state__ATrue(!info.isNonNativeElf);
    string__AEqual(TERMUX__PREFIX__BIN_DIR "/sh", info.interpreterPath);
    string__AEqual("-x", info.interpreterArg);

    snprintf(header, hsize, "#!/bin/sh -x \n") < 0 ? abort() : (void)0;
    inspectFileHeader(TERMUX__PREFIX, header, sizeof(header) , &info);
    state__ATrue(!info.isElf);
    state__ATrue(!info.isNonNativeElf);
    string__AEqual(TERMUX__PREFIX__BIN_DIR "/sh", info.interpreterPath);
    string__AEqual("-x", info.interpreterArg);

    info.interpreterPath = NULL;
    info.interpreterArg = NULL;
    // An ELF header for a 32-bit file.
    // See https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#File_header
    snprintf(header, hsize, "\177ELF") < 0 ? abort() : (void)0;
    // Native instruction set.
    header[0x12] = EM_NATIVE;
    header[0x13] = 0;
    inspectFileHeader(TERMUX__PREFIX, header, sizeof(header) , &info);
    state__ATrue(info.isElf);
    state__ATrue(!info.isNonNativeElf);
    state__ATrue(info.interpreterPath == NULL);
    state__ATrue(info.interpreterArg == NULL);

    info.interpreterPath = NULL;
    info.interpreterArg = NULL;
    // An ELF header for a 64-bit file.
    // See https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#File_header
    snprintf(header, hsize, "\177ELF") < 0 ? abort() : (void)0;
    // 'Fujitsu MMA Multimedia Accelerator' instruction set - likely non-native.
    header[0x12] = 0x36;
    header[0x13] = 0;
    inspectFileHeader(TERMUX__PREFIX, header, sizeof(header) , &info);
    state__ATrue(info.isElf);
    state__ATrue(info.isNonNativeElf);
    state__ATrue(info.interpreterPath == NULL);
    state__ATrue(info.interpreterArg == NULL);
}





void test__modifyExecEnv__unsetLDVars();
void test__modifyExecEnv__setProcSelfExe();

void test__modifyExecEnv() {
    logVerbose(LOG_TAG, "test__modifyExecEnv()");

    test__modifyExecEnv__unsetLDVars();
    test__modifyExecEnv__setProcSelfExe();

    int__AEqual(0, errno);
}

void test__modifyExecEnv__unsetLDVars() {
    logVVerbose(LOG_TAG, "test__modifyExecEnv__unsetLDVars()");

    {
    char *testEnv[] = {"MY_ENV=1", NULL};
    char **allocatedEnvp;
    modifyExecEnv(testEnv, &allocatedEnvp, NULL, true);
    state__ATrue(allocatedEnvp != NULL);
    string__AEqual(allocatedEnvp[0], "MY_ENV=1");
    state__ATrue(allocatedEnvp[1] == NULL);
    free(allocatedEnvp);
    }

    {
    char *testEnv[] = {"MY_ENV=1", ENV_PREFIX__LD_PRELOAD "a", NULL};
    char **allocatedEnvp;
    modifyExecEnv(testEnv, &allocatedEnvp, NULL, true);
    state__ATrue(allocatedEnvp != NULL);
    string__AEqual(allocatedEnvp[0], "MY_ENV=1");
    state__ATrue(allocatedEnvp[1] == NULL);
    free(allocatedEnvp);
    }

    {
    char *testEnv[] = {"MY_ENV=1", ENV_PREFIX__LD_PRELOAD "a", "A=B", ENV_PREFIX__LD_LIBRARY_PATH "B", "B=C", NULL};
    char **allocatedEnvp;
    modifyExecEnv(testEnv, &allocatedEnvp, NULL, true);
    state__ATrue(allocatedEnvp != NULL);
    string__AEqual(allocatedEnvp[0], "MY_ENV=1");
    string__AEqual(allocatedEnvp[1], "A=B");
    string__AEqual(allocatedEnvp[2], "B=C");
    state__ATrue(allocatedEnvp[3] == NULL);
    free(allocatedEnvp);
    }
}

void test__modifyExecEnv__setProcSelfExe() {
    logVVerbose(LOG_TAG, "test__modifyExecEnv__setProcSelfExe()");

    {
    char *termuxProcSelfExe = NULL;
    state__ATrue(asprintf(&termuxProcSelfExe, "%s%s", ENV_PREFIX__TERMUX_EXEC__PROC_SELF_EXE, TERMUX__PREFIX__BIN_DIR "/bash") != -1);

    char *testEnv[] = {"MY_ENV=1", NULL};
    char **allocatedEnvp;
    modifyExecEnv(testEnv, &allocatedEnvp, &termuxProcSelfExe, false);
    state__ATrue(allocatedEnvp != NULL);
    string__AEqual(allocatedEnvp[0], "MY_ENV=1");
    string__AEqual(allocatedEnvp[1], ENV__TERMUX_EXEC__PROC_SELF_EXE "=" TERMUX__PREFIX__BIN_DIR "/bash");
    free(termuxProcSelfExe);
    free(allocatedEnvp);
    }

    {
    char *termuxProcSelfExe = NULL;
    state__ATrue(asprintf(&termuxProcSelfExe, "%s%s", ENV_PREFIX__TERMUX_EXEC__PROC_SELF_EXE, TERMUX__PREFIX__BIN_DIR "/bash") != -1);

    char *testEnv[] = {"MY_ENV=1", ENV__TERMUX_EXEC__PROC_SELF_EXE "=" TERMUX__PREFIX__BIN_DIR "/python", NULL};
    char **allocatedEnvp;
    modifyExecEnv(testEnv, &allocatedEnvp, &termuxProcSelfExe, false);
    state__ATrue(allocatedEnvp != NULL);
    string__AEqual(allocatedEnvp[0], "MY_ENV=1");
    string__AEqual(allocatedEnvp[1], ENV__TERMUX_EXEC__PROC_SELF_EXE "=" TERMUX__PREFIX__BIN_DIR "/bash");
    free(termuxProcSelfExe);
    free(allocatedEnvp);
    }

    {
    char *testEnv[] = {"MY_ENV=1", NULL};
    char **allocatedEnvp;
    modifyExecEnv(testEnv, &allocatedEnvp, NULL, false);
    state__ATrue(allocatedEnvp != NULL);
    string__AEqual(allocatedEnvp[0], "MY_ENV=1");
    state__ATrue(allocatedEnvp[1] == NULL);
    free(allocatedEnvp);
    }

    {
    char *testEnv[] = {"MY_ENV=1", ENV__TERMUX_EXEC__PROC_SELF_EXE "=" TERMUX__PREFIX__BIN_DIR "/python", NULL};
    char **allocatedEnvp;
    modifyExecEnv(testEnv, &allocatedEnvp, NULL, false);
    state__ATrue(allocatedEnvp != NULL);
    string__AEqual(allocatedEnvp[0], "MY_ENV=1");
    state__ATrue(allocatedEnvp[1] == NULL);
    free(allocatedEnvp);
    }
}
