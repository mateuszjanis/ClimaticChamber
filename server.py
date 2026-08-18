from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/get_data', methods=['GET'])
def handle_get():
    print("Otrzymano zapytanie GET!")
    return jsonify({"status": "sukces", "wiadomosc": "Witaj z Raspberry Pi (GET)"}), 200

@app.route('/post_data', methods=['POST'])
def handle_post():
    data = request.data.decode('utf-8')
    print(f"Otrzymano zapytanie POST! Dane: {data}")
    return jsonify({"status": "sukces", "otrzymane_dane": data}), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)