#pragma once

#ifndef _STRATEGY_STUDIO_XGBOOST_STRATEGY_H_
#define _STRATEGY_STUDIO_XGBOOST_STRATEGY_H_

#include <Strategy.h>
#include <MarketModels/Instrument.h>
#include <xgboost/c_api.h>
#include <string>
#include <vector>

namespace RCM::StrategyStudio {

class XGBoost : public Strategy {
public:
    // Constructor and Destructor
    XGBoost(StrategyID strategyID,
            const std::string& strategyName,
            const std::string& groupName);
    ~XGBoost();

    // Event Handlers
    void OnTrade(const TradeDataEventMsg& msg) override;
    void OnOrderUpdate(const OrderUpdateEventMsg& msg) override {}
    void OnBar(const BarEventMsg& msg) override {}
    void OnResetStrategyState() override {}
    void OnParamChanged(StrategyParam& param) override {}

private:
    // Private Members
    BoosterHandle booster;   // XGBoost model handle
    int current_trade = 0;   // Counter to control trade frequency

    // Helper Methods
    void LoadModel(const std::string& model_path);                  // Load XGBoost model
    std::vector<double> ExtractFeatures(const TradeDataEventMsg& msg); // Extract features from market data
    std::pair<bool, int> Predict(const std::vector<double>& features); // Predict buy/sell signal

    // Order Management
    void SendSimpleOrder(const Instrument* instrument, int trade_size);
};

} // namespace RCM::StrategyStudio

#endif // _STRATEGY_STUDIO_XGBOOST_STRATEGY_H_
