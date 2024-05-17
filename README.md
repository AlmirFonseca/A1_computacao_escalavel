# Data Framework

The Data Framework is a C++ project that provides a set of functions for handling data processing pipelines, focusing on concurrent and parallel execution. With various functions available, users can perform several useful operations for data wrangling.

## Features:

The project is based on a data processing pipeline modeling with 5 abstractions:

1. Dataframe: a tabular data structure that stores the data to be processed.
2. Handler: a function that processes the data from a Dataframe.
3. Repository: a data structure that stores the Dataframes.
4. Trigger: a function that triggers the execution of a Handler.
5. Queue: a data structure that stores the input and output Dataframes of a Handler.

To optimize the use of computing resources, Data Framework uses the C++ thread library to parallelize the execution of Handlers. This allows for more efficient and faster data processing. Specifically, queues serve as producer-consumer queues, addressing the classic problem of parallel computing.

## Pre-Requirements:

- C++ compiler (version 11+)
- Python 3.11+

## Libraries used:

- networkx
  - pip install networkx

- faker
  - pip install faker

- GRPC para Python:
  - https://grpc.io/docs/languages/python/quickstart/

- GRPC para C++:
  - [IN CASE OF USING WINDOWS, INSTALL THE WSL [https://learn.microsoft.com/pt-br/windows/wsl/install] AS IT IS THE EASIEST WAY TO ]
  - https://grpc.io/docs/languages/cpp/quickstart/

- protocol buffers:
  - https://grpc.io/docs/protoc-installation/

- libncursesw5-dev:
  - sudo apt-get install libncursesw5-dev

- streamlit

  - pip install streamlit
- sqlite 3:

  - [https://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm](https://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm)
- dirent.h (most c++ compilers include it, so install it just if the main requires):

  - Installation guide: [https://web.archive.org/web/20171123102131/https://github.com/tronkko/dirent]()

## Execution on Windows:

1. First, run the proto:

   1. From the root folder, generate the scripts from proto for CLIENT:

      - cd shared_proto ;
      - python -m grpc_tools.protoc --proto_path=. ./data_analytics.proto --python_out=../mock --grpc_python_out=../mock

   2. (SERVER IN C++) Open a new terminal on root to execute the server (in Linux/WSL) as follows:
      - cd src ; make ; ./server

      - [OPTIONAL] SERVER IN PYTHON AVAILABE AFTER 
         - python -m grpc_tools.protoc --proto_path=. ./data_analytics.proto --python_out=../src --grpc_python_out=../src
      

    3. Open a new terminal on root to execute the client as follows:
      - cd mock ; python client.py
2. Run the Mock and the ETL:

- The first alternative is to run the shell script:

  - On the root folder type:
    - `run_it.sh`
- Another alternative:

  - **From the root folder**, execute the command to run the MOCK:

    - `cd mock ; python main.py`
  - **From the root folder**, execute the command run the ETL:

    - `cd src ; g++ main.cpp -o main ; ./main.exe`

2. Run the Dashboard:

- **From the root folder**:

  `cd dashboard; - python -m streamlit run main.py`
