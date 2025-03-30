import pyshark
import json
import csv

class BtcPcapParser:
    def __init__(self):
        # Define the pcap file to parse
        self.pcap_file = '20241112_IEXTP1_DEEP1.0.pcap'  # Replace with your pcap file name
        self.output_csv = 'btc_trades.csv'
        self.symbol_of_interest = 'AAPL'
        self.total_packets = 0
        self.total_trades = 0

    def parse(self):
        print(f'Parsing pcap file: {self.pcap_file}')
        # Open the pcap file using PyShark
        capture = pyshark.FileCapture(self.pcap_file, display_filter='tcp or websocket')
        with open(self.output_csv, 'w', newline='') as csvfile:
            csv_writer = csv.writer(csvfile)
            csv_writer.writerow(['Timestamp', 'Symbol', 'Price', 'Size', 'Trade ID'])

            for packet in capture:
                self.total_packets += 1
                # Provide status updates every 1000 packets
                if self.total_packets % 1000 == 0:
                    print(f'Processed {self.total_packets} packets, {self.total_trades} BTC trades found.')

                try:
                    # Check if the packet has WebSocket layer
                    if hasattr(packet, 'websocket'):
                        self.parse_websocket_packet(packet, csv_writer)
                    else:
                        # If not, check the TCP payload
                        self.parse_tcp_packet(packet, csv_writer)
                except Exception as e:
                    # Handle any parsing exceptions
                    pass  # You can add logging here if needed

        capture.close()
        print(f'Parsing complete. Total packets: {self.total_packets}, Total BTC trades: {self.total_trades}')
        print(f'Trade data saved to {self.output_csv}')

    def parse_websocket_packet(self, packet, csv_writer):
        # Extract the WebSocket payload
        payload = packet.websocket.payload
        payload_bytes = bytes.fromhex(payload.replace(':', ''))
        payload_str = payload_bytes.decode('utf-8', errors='ignore')
        self.process_payload(payload_str, packet.sniff_time, csv_writer)

    def parse_tcp_packet(self, packet, csv_writer):
        # Extract TCP payload
        if hasattr(packet.tcp, 'payload'):
            payload = packet.tcp.payload
            payload_bytes = bytes.fromhex(payload.replace(':', ''))
            payload_str = payload_bytes.decode('utf-8', errors='ignore')
            self.process_payload(payload_str, packet.sniff_time, csv_writer)

    def process_payload(self, payload_str, timestamp, csv_writer):
        # Parse the payload as JSON
        try:
            data = json.loads(payload_str)
            # Check if the message contains BTC data
            if self.is_btc_trade(data):
                self.total_trades += 1
                # Extract relevant fields
                symbol = data.get('symbol', 'BTC')
                price = data.get('price')
                size = data.get('size')
                trade_id = data.get('trade_id', '')
                # Write to CSV
                csv_writer.writerow([timestamp, symbol, price, size, trade_id])
        except json.JSONDecodeError:
            # Not a JSON payload, skip
            pass

    def is_btc_trade(self, data):
        # Implement logic to determine if the data represents a BTC trade
        if 'symbol' in data and data['symbol'] == self.symbol_of_interest:
            return True
        return False

if __name__ == '__main__':
    parser = BtcPcapParser()
    parser.parse()
