from twilio.rest import Client

account_sid = 'ACd95d3c60b1a7f0703f790bc29c5bcf63'
auth_token = 'b6172e7bddb272bccbf40f95f5e89137'
client = Client(account_sid, auth_token)

message = client.messages.create(
	from_='whatsapp:+14155238886',
  	body='¡Alerta! Threshold alcanzado. El cliente ha recibido 5 veces la salida del servidor',
  	to='whatsapp:+593960603364'
)
