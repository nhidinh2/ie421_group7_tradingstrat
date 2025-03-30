#include "XGBoost.hpp"
#include <xgboost/c_api.h>
#include <iostream>
#include <vector>

const std::string path_to_model = "models/xgb_model.json";

XGBoost::XGBoost(StrategyID strategyID,
                 const std::string& strategyName,
                 const std::string& groupName)
    : Strategy(strategyID, strategyName, groupName), booster(nullptr) {
    try {
        LoadModel(path_to_model);
    } catch (const std::exception& e) {
        std::cerr << "Error loading the model: " << e.what() << "\n";
    }
}

XGBoost::~XGBoost() {
    if (booster) {
        XGBoosterFree(booster);
    }
}

void XGBoost::LoadModel(const std::string& model_path) {
    if (XGBoosterCreate(nullptr, 0, &booster) != 0) {
        throw std::runtime_error("Failed to create XGBoost booster.");
    }
    if (XGBoosterLoadModel(booster, model_path.c_str()) != 0) {
        throw std::runtime_error("Failed to load XGBoost model.");
    }
}

std::vector<double> XGBoost::ExtractFeatures(const TradeDataEventMsg& msg) {
    return {msg.price(), msg.quantity(), msg.instrument().last_trade().price()};
}

std::pair<bool, int> XGBoost::Predict(const std::vector<double>& features) {
    DMatrixHandle dmatrix;
    if (XGDMatrixCreateFromMat(features.data(), 1, features.size(), NAN, &dmatrix) != 0) {
        throw std::runtime_error("Failed to create DMatrix.");
    }

    bst_ulong out_len;
    const float* out_result;
    if (XGBoosterPredict(booster, dmatrix, 0, 0, 0, &out_len, &out_result) != 0) {
        XGDMatrixFree(dmatrix);
        throw std::runtime_error("Failed to predict with XGBoost model.");
    }

    XGDMatrixFree(dmatrix);

    bool buy = out_result[0] > 0.5; // Example threshold
    int trade_size = 100;          // Example trade size
    return {buy, trade_size};
}

void XGBoost::OnTrade(const TradeDataEventMsg& msg) {
    if (current_trade % 6000 == 0) {
        try {
            std::vector<double> features = ExtractFeatures(msg);
            std::pair<bool, int> prediction = Predict(features);

            if (prediction.first) {
                SendSimpleOrder(&msg.instrument(), prediction.second);
            } else {
                SendSimpleOrder(&msg.instrument(), -prediction.second);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error during prediction: " << e.what() << "\n";
        }
    }
    current_trade++;
}


void XGBoost::OnOrderUpdate(const OrderUpdateEventMsg& msg) {
}

void XGBoost::OnBar(const BarEventMsg& msg) {
}

void XGBoost::AdjustPortfolio() {
}

void XGBoost::SendSimpleOrder(const Instrument* instrument,
int trade_size) {
    // send order two pennies more aggressive than BBO
    double m_aggressiveness = 0.02;
    double last_trade_price = instrument->last_trade().price();
    double price = trade_size > 0 ? last_trade_price + m_aggressiveness :
    last_trade_price - m_aggressiveness;
    OrderParams params(*instrument,
        abs(trade_size),
        price,
        (instrument->type() == INSTRUMENT_TYPE_EQUITY) ? MARKET_CENTER_ID_IEX :
        ((instrument->type() == INSTRUMENT_TYPE_OPTION) ?
        MARKET_CENTER_ID_CBOE_OPTIONS : MARKET_CENTER_ID_CME_GLOBEX),
        (trade_size > 0) ? ORDER_SIDE_BUY : ORDER_SIDE_SELL,
        ORDER_TIF_DAY,
        ORDER_TYPE_LIMIT);

    std::cout << "SendSimpleOrder(): about to send new order for " <<
     trade_size << " at $" << price << std::endl;
    TradeActionResult tra = trade_actions()->SendNewOrder(params);
    if (tra == TRADE_ACTION_RESULT_SUCCESSFUL) {
        // m_instrument_order_id_map[instrument] = params.order_id;
        std::cout << "SendOrder(): Sending new order successful!" << std::endl;
    } else {
        std::cout << "SendOrder(): Error sending new order!!!" << tra
        << std::endl;
    }
}


void XGBoost::SendOrder(const Instrument* instrument, int trade_size) {
    
}

void XGBoost::OnResetStrategyState() {
}

void XGBoost::OnParamChanged(StrategyParam& param) {
}
