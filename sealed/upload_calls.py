import pytz
import getopt
import json
import logging
import os
import shutil
import sys
import threading
from time import sleep
from datetime import datetime
import requests


def upload_file_trampoline(uploader, file):
    uploader.upload_file(file)


def do_http_upload(files, headers, upload_url):
    try:
        tz = pytz.timezone('Europe/Athens')
        ts = datetime.now(tz=tz).timestamp()
        utc_offset = (datetime.fromtimestamp(ts) -
                      datetime.utcfromtimestamp(ts)).total_seconds()

        payload = {"utc_offset": int(utc_offset),
                   "force_stereo": True}

        metadata = {
            'a': 1,
            'b': 2,
            'c': 3
        }

        response = requests.request("POST", upload_url, headers=headers, files=files, params=payload, verify=True,
                                    data=metadata)
        if response.status_code == 201:
            print(response.text.encode('utf8'))
            try:
                response = json.loads(response.text)
                session_id = json.loads(response["status"])["session_id"]
            except:
                return False, None
            return True, session_id

        if response.status_code == 429:
            print("No place left on device, waiting")

    except ConnectionResetError as e:
        print("Connection rest error - File too long?")
        print(e)

    return False, None


class FilesUploader:
    def __init__(self, server_url, username, password, count=10):
        self._semaphore = threading.Semaphore(count)
        self._token = None
        self._save_json_links = False
        self._url = server_url
        self._directory = "/root/recordings"
        self._username = username
        self._password = password
        self._otp = '432039'

    def login(self):
        url_login = f'{self._url}/api/login/'
        payload = {
            "username": self._username,
            "password": self._password,
            "send_otp_by_mail": False}
        payload = json.dumps(payload)
        headers = {
            'Content-Type': 'application/json'
        }

        requests.request("POST", url_login, headers=headers, data=payload, verify=True)
        request_verify_otp = {
            "username": self._username,
            "otp": self._otp
        }
        payload = json.dumps(request_verify_otp)
        url_verify = f'{self._url}/api/login/otp'
        result = requests.request("POST", url_verify, headers=headers, data=payload, verify=True)
        response = json.loads(result.text)
        print("login succeeded:%s" % response["token"])
        self._token = response["token"]

    def upload_file(self, file_path):
        upload_url = self._url + "/api/upload/call"

        if FilesUploader.file_big_enough(file_path) and FilesUploader.valid_call(file_path):
            files = [
                ('recording',
                 open(file_path, 'rb'))
            ]

            token = self._token

            authorization = 'Bearer ' + token

            headers = {
                'Authorization': authorization
            }

            print("Uploading file:{}\n".format(file_path))

            success, session_id = do_http_upload(files, headers, upload_url)
            if success:
                logging.info('success')
                os.remove(file_path)
            else:
                shutil.move(file_path,"/root/recordings")
        else:
            print(f"filtering:{file_path}")
            os.remove(file_path)

    def upload_files_from_directory(self, files_dir):
        try:
            threads_queue = []
            for filename in os.listdir(files_dir):
                t = threading.Thread(target=upload_file_trampoline,
                                     name='thread_' + filename,
                                     args=(self, files_dir + "/" + filename))
                print(f'files_dir + "/" + filename = {files_dir + "/" + filename}')
                t.start()
                threads_queue.append(t)
                if len(threads_queue)==20:
                    threads_queue[0].join()
                    threads_queue=threads_queue[1:]

        except NotADirectoryError as e:
            print(e)

    def do_upload(self):
        self.login()

        destination_folder = "/root/active_uploads"

        if not os.path.exists(destination_folder):
            os.mkdir(destination_folder)

        for file_name in os.listdir(self._directory):
            # construct full file path
            print(f"Copying files:{file_name}")
            source = f"{self._directory}/{file_name}"
            destination = f"{destination_folder}/{file_name}"
            shutil.move(source, destination)

        self.upload_files_from_directory(destination_folder)

    @classmethod
    def file_big_enough(cls, upload_file):
        import soundfile as sf
        f = sf.SoundFile(upload_file)
        duration = len(f) / f.samplerate
        return duration > 40

    def send_utilization_report(self):
        result = os.statvfs('/')
        total_blocks = result.f_blocks
        free_blocks = result.f_bfree
        disk_utilization = int(100 * ((total_blocks - free_blocks) / total_blocks))

        stream = os.popen('mpstat')
        cpu_utilization_data = stream.read()
        lines = cpu_utilization_data.split(sep='\n')
        columns = lines[2].split()
        row = lines[3].split()
        cpu_utilization_dict = {}
        for c, r in zip(columns, row):
            if '%' in c and 'idle' not in c:
                cpu_utilization_dict[c] = r
        cpu_utilization = int(sum(float(v) for v in cpu_utilization_dict.values()))

        data = {
            "cpu_utilization" : cpu_utilization,
            "disk_utilization": disk_utilization
        }

        self.login()

        token = self._token

        authorization = 'Bearer ' + token

        headers = {
            'Authorization': authorization
        }

        health_url = self._url + "/api/upload/sniffer_health_report"

        requests.request("POST", health_url, headers=headers, verify=True,
                                    data=data)

    @classmethod
    def valid_call(cls, file_path):
        file_parts = file_path.split("-")
        if file_parts[1]=="anonymous":
            return False
        if file_parts[1].split(".")==4:
            return False
        if file_parts[1]=="asterisk":
            return False
        if len(file_parts[1])<4:
            return False
        if len(file_parts[1])>5:
            return False
        print(f"Valid call name:{file_path}")
        return True

def upload(argv):
    url = None
    username = None
    password = None

    try:
        opts, args = getopt.getopt(argv, "s:d:u:p:l:", ["server=", "username=", "password="])
    except getopt.GetoptError:
        print("call_upload -s <destination_url> -d <Local directory> -uname <username> -p <password>")
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print("call_upload -url <destination_url> -d <Local directory> -u <username> -p <password>")
            sys.exit()
        elif opt in ("-s", "--server"):
            url = arg
        elif opt in ("-u", "--username"):
            username = arg
        elif opt in ("-p", "--password"):
            password = arg

    if url is None:
        print("Need url list")
        sys.exit()

    fu = FilesUploader(server_url=url, username=username, password=password)

    count = 0
    while True:
        fu.do_upload()
        sleep(60)
#        if count%10==0:
#            fu.send_utilization_report()
        count+=1

if __name__ == '__main__':
    upload(sys.argv[1:])