# Conditional settings based on passed in variables
ifdef INTEL
    CC=icc
else
    CC=g++
endif

# Path to XGBoost C API
XGBOOST_PATH=/home/vagrant/ss/sdk/RCM/StrategyStudio/includes/xgboost

# Compilation flags
ifdef DEBUG
    CFLAGS=-c -g -fPIC -fpermissive -std=c++11
else
    CFLAGS=-c -fPIC -fpermissive -std=gnu++14
endif

LIBPATH=../../../libs/x64
INCLUDEPATH=../../../includes

INCLUDES=-I/usr/include \
         -I$(INCLUDEPATH) \
         -I$(XGBOOST_PATH)/include
         
LDFLAGS=$(LIBPATH)/libstrategystudio_analytics.a \
        $(LIBPATH)/libstrategystudio.a \
        $(LIBPATH)/libstrategystudio_transport.a \
        $(LIBPATH)/libstrategystudio_marketmodels.a \
        $(LIBPATH)/libstrategystudio_utilities.a \
        $(LIBPATH)/libstrategystudio_flashprotocol.a \
        -L$(XGBOOST_PATH)/lib -lxgboost

LIBRARY=XGBoost.so

SOURCES=XGBoost.cpp
HEADERS=XGBoost.hpp

OBJECTS=$(SOURCES:.cpp=.o)

all: $(HEADERS) $(LIBRARY)

$(LIBRARY) : $(OBJECTS)
	$(CC) -shared -Wl,-soname,$(LIBRARY).1 -o $(LIBRARY) $(OBJECTS) $(LDFLAGS)
	
.cpp.o: $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -rf *.o $(LIBRARY)
