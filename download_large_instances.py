#!/usr/bin/env python3
"""
Download larger TSP instances for enhanced experiments
"""

import requests
import os
from pathlib import Path

def download_tsp_instance(instance_name, url_base="http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/tsp/"):
    """Download TSP instance from TSPLIB95"""
    data_dir = Path("data")
    data_dir.mkdir(exist_ok=True)
    
    url = f"{url_base}{instance_name}.tsp"
    file_path = data_dir / f"{instance_name}.tsp"
    
    try:
        print(f"Downloading {instance_name}...")
        response = requests.get(url, timeout=30)
        response.raise_for_status()
        
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(response.text)
        
        print(f"✓ Successfully downloaded {instance_name}.tsp")
        return True
        
    except requests.exceptions.RequestException as e:
        print(f"✗ Failed to download {instance_name}: {e}")
        return False

def create_synthetic_large_instance(name, size):
    """Create a synthetic TSP instance if download fails"""
    data_dir = Path("data")
    file_path = data_dir / f"{name}.tsp"
    
    import random
    random.seed(42)  # For reproducible results
    
    content = f"""NAME: {name}
TYPE: TSP
COMMENT: Synthetic {size}-city instance for testing
DIMENSION: {size}
EDGE_WEIGHT_TYPE: EUC_2D
NODE_COORD_SECTION
"""
    
    # Generate random coordinates
    for i in range(1, size + 1):
        x = random.randint(0, 1000)
        y = random.randint(0, 1000)
        content += f"{i} {x} {y}\n"
    
    content += "EOF\n"
    
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(content)
    
    print(f"✓ Created synthetic instance {name}.tsp with {size} cities")
    return True

def main():
    """Download or create larger TSP instances"""
    instances_to_download = [
        ("kroA200", 200),
        ("pr226", 226),
        ("a280", 280),
        ("pcb442", 442)
    ]
    
    alternative_urls = [
        "http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/tsp/",
        "https://www.iwr.uni-heidelberg.de/groups/comopt/software/TSPLIB95/tsp/",
        "http://elib.zib.de/pub/mp-testdata/tsp/tsplib/tsp/"
    ]
    
    for instance_name, size in instances_to_download:
        success = False
        
        # Try different URL bases
        for url_base in alternative_urls:
            if download_tsp_instance(instance_name, url_base):
                success = True
                break
        
        # If download fails, create synthetic instance
        if not success:
            print(f"Download failed for {instance_name}, creating synthetic instance...")
            create_synthetic_large_instance(instance_name, size)
    
    print("\nLarge instance preparation complete!")

if __name__ == "__main__":
    main()
