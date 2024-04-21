# A1_computacao_escalavel

**Libraries used**:

- dirent.h (JUST INSTALL IF THE MAIN REQUIRES IT, as most c++ compilers include it)
  - Installation guide: https://web.archive.org/web/20171123102131/https://github.com/tronkko/dirent

**Execution on Windows**:

The first alternative is to run the shell script "run_it.sh".

Another alternative:

- **From the root folder**, execute the command to run the MOCK:
  - cd mock ; python main.py
- **From the root folder**, execute the command run the ETL:
  - cd src ; g++ main.cpp -o main ; ./main.exe
