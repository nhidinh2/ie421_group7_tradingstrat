#!/bin/bash
LIBRARY=strategy.so

# copy_strategy
cp $LIBRARY /home/vagrant/ss/bt/strategies_dlls/

# launch_backtest
cd /home/vagrant/ss/bt && ./StrategyServerBacktesting &

echo "Sleeping for 2 seconds while waiting for strategy studio to boot"
sleep 2

cd /home/vagrant/ss/bt/utilities 
./StrategyCommandLine cmd create_instance strategy strategy UIUC SIM-1001-101 dlariviere 1000000 -symbol GBTC 
./StrategyCommandLine cmd strategy_instance_list 

# run_backtest
(cd /home/vagrant/ss/bt && ./StrategyServerBacktesting&)
echo "Sleeping for 2 seconds while waiting for strategy studio to boot"
sleep 2
./StrategyCommandLine cmd start_backtest 2024-11-19 2024-11-19 strategy 1


#output_results: 
export CRA_RESULT=`cd /home/vagrant/ss/bt/backtesting-results
find ./backtesting-results -name 'BACK*cra' | tail -n 1`
echo $CRA_RESULT