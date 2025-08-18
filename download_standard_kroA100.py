import urllib.request
import os

def download_standard_kroA100():
    """下載標準的 kroA100 TSPLIB 實例"""
    
    base_url = "http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95"
    
    # 要下載的文件
    files = [
        ("tsp/kroA100.tsp", "data/kroA100_standard.tsp"),
        ("optimal/kroA100.opt.tour", "data/kroA100_standard.opt.tour")
    ]
    
    for url_path, local_path in files:
        url = f"{base_url}/{url_path}"
        print(f"Downloading {url} to {local_path}")
        
        try:
            urllib.request.urlretrieve(url, local_path)
            print(f"Successfully downloaded {local_path}")
        except Exception as e:
            print(f"Error downloading {url}: {e}")

if __name__ == "__main__":
    download_standard_kroA100()
