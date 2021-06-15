import requests, rsa, uuid, lzstring, csv, datetime, os

class n_login(object):
    def __init__(self, id, pw):
        def get_show_id(id):
            return id[0:int(len(id) / 2)] + ''.join(
                [id[z].replace(id[z], "*") for z in range(int(len(id) / 2), len(id))])

        def get_show_pw(pw):
            return ''.join([pw[z].replace(pw[z], "*") for z in range(len(pw))])

        show_id = get_show_id(id)
        show_pw = get_show_pw(pw)
        print(f"Your Id: {show_id}")
        print(f"Your Pw: {show_pw}")

    def get_session(self, id, pw):
        def encrypt(keys2_nhn, id, pw):
            sessionkey, keyname, e_value, n_value = keys2_nhn.split(',')

            enctext = enc_source([sessionkey, id, pw]).encode()
            e, n = int(e_value, 16), int(n_value, 16)

           
            pub_key = rsa.PublicKey(e, n)

            return keyname, rsa.encrypt(enctext, pub_key)

        def enc_source(source_list):
            return "".join([chr(len(s)) + s for s in source_list])

        def get_sign_in_naver_url(encnm, encpw):
            hdr = {
                'user-agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.182 Safari/537.36 Edg/88.0.705.81'
            }

            with requests.Session() as s:
                bvsd_uuid = uuid.uuid4()
                bvsd_uuid = "hi123"

                encData = '{"a":"%s-4","b":"1.3.4","d":[{"i":"id","b":{"a":["0,%s"]},"d":"%s","e":false,"f":false},{"i":"%s","e":true,"f":false}],"h":"1f","i":{"a":"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.182 Safari/537.36 Edg/88.0.705.81"}}' % (bvsd_uuid, id, id, pw)


                bvsd = '{"uuid":"%s", "encData":"%s"}' % (bvsd_uuid, lzstring.LZString.compressToEncodedURIComponent(encData))
                print(encData)
                print(bvsd)
                print(encpw.hex())

                Post_Data = {
                    'encnm' : encnm,
                    'encpw' : encpw.hex(),
                    'bvsd' : bvsd
                }
                res = s.post('https://nid.naver.com/nidlogin.login', data=Post_Data, headers=hdr)
                print('@@@@@@@@@' + res.text)
                if not res.status_code == 200:
                    raise Exception(f"Connect Error\nError Code: {res.status_code}")
                elif res.text.count("\n") > 15:
                    raise Exception("Login Fail. Please check your Id and Pw.")
                else:
                    try:
                        login_url = res.text[res.text.find("replace(\"")+9:res.text.find("\");")].strip()

                        s.get(login_url, headers=hdr)

                        #user_data_to_csv(id, pw, s)
                        return s
                    except:
                        raise Exception("Unexpected Error")



        keys2_nhn = requests.get("https://nid.naver.com/login/ext/keys2.nhn").content.decode('utf-8')

        if keys2_nhn == None:
            return None
        else:
            keyname, encpw = encrypt(keys2_nhn, id, pw)

            if keyname == None or encpw == None:
                return None
            else:
                print("Naver Login Success!")
                return get_sign_in_naver_url(keyname, encpw)


id='memscan'
pw='wbeltmtjq2'
a = n_login(id, pw)
with a.get_session(id, pw) as s:
  print(s.get("https://www.naver.com")) 