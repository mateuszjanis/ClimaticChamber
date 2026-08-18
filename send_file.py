import os
import paramiko

def sftp_upload(local_file_path, remote_file_path, hostname, username, key_path=None, password=None):

	"""
	Przesyłanie plików rpi -> windows (paramiko)
	"""

	ssh = paramiko.SSHClient()

	ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())

	try:
		print(f"Łączenie z {hostname}...")

		if key_path:
			k = paramiko.Ed25519Key.from_private_key_file(key_path)
			ssh.connect(hostname, username=username, pkey=k, timeout=10)

		else:
			ssh.connect(hostname, username=username, password=password, timeout=10)

			print("Połączono. Otwieranie sesji SFTP...")
			sftp = ssh.open_sftp()

			print(f"Wysyłanie: {local_file_path} -> {remote_file_path}")
			sftp.put(local_file_path, remote_file_path)

			print("Transfer zakończony sukcesem")	
			sftp.close()

	except Exception as e:
		print(f"Wystąpił błąd podczas transferu {e}")

	finally:
		ssh.close()
		print("Połączenie SSH zamknięte.")


WINDOWS_IP = "192.168.8.122"
WINDOWS_USER = "matis"

PLIK_RPI = "test_img.jpg"
PLIK_WINDOWS = "C:/Users/matis/OneDrive/Dokumenty/WIMIR/PRACA_INZYNIERSKA/TESTY"

KLUCZ_PRYWATNY = os.path.expanduser("~/.ssh/id_ed25519")

if __name__ == "__main__":

	sftp_upload(PLIK_RPI, PLIK_WINDOWS, WINDOWS_IP, WINDOWS_USER, key_path=KLUCZ_PRYWATNY)
