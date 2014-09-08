# Macros
#

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),FreeBSD)
CC = gcc46 
else
CC = gcc 
endif

CFLAGS = -c -Wall -ggdb  
LIB = -lm

SRCEVENGINE = evengine/calendar.c evengine/event.c evengine/eventmgr.c 

SRCINFRASTRUCTURE = infrastructure/broker.c infrastructure/datacenter.c infrastructure/fattree.c infrastructure/network.c infrastructure/node.c infrastructure/provider.c infrastructure/scaler.c infrastructure/balancer.c infrastructure/vm.c infrastructure/request_mgr.c

SRCUTILS = utils/get_conf.c utils/literal.c utils/misc.c utils/rand.c 

SRCWORKLOADS = workloads/workload.c workloads/application.c workloads/requests.c 

SRCSTATS = stats/log.c stats/metrics.c stats/batch.c

SRCEVALUATION = evaluation/functions.c

SRCMODELS = models/energy_models.c

SRCOPTIMIZATION = optimization/opt_engine.c optimization/opt_selector.c optimization/opt_variator.c optimization/problem.c optimization/nsga2.c optimization/spea2.c optimization/hype.c optimization/shv.c optimization/mutation.c optimization/crossover.c 

SRCMAIN = main.c

OBJS = $(SRCEVENGINE:.c=.o) $(SRCINFRASTRUCTURE:.c=.o) $(SRCUTILS:.c=.o) $(SRCWORKLOADS:.c=.o) $(SRCSTATS:.c=.o) $(SRCEVALUATION:.c=.o) $(SRCMODELS:.c=.o) $(SRCOPTIMIZATION:.c=.o) $(SRCMAIN:.c=.o)

EXECUTABLE = ../bin/cse
# Explicit rules
all: $(SRCEVENGINE) $(SRCINFRASTRUCTURE) $(SRCUTILS) $(SRCMAIN) $(SRCWORKLOADS) $(SRCSTATS) $(SRCEVALUATION) $(SRCMODELS) $(SRCOPTIMIZATION) $(EXECUTABLE) 

$(EXECUTABLE): $(OBJS)
	$(CC) $(OBJS) $(LIB) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@	

clean:  
	rm $(OBJS) ../bin/cse


