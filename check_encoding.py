import os
import sys
import locale

# 검사 대상 확장자 정의 (대소문자 구분 없음)
TARGET_EXTENSIONS = ('.cpp', '.h')

def get_file_size_str(file_path):
    """파일 크기를 읽기 쉬운 단위(B, KB, MB)로 변환"""
    try:
        size_bytes = os.path.getsize(file_path)
        if size_bytes < 1024:
            return f"{size_bytes} B"
        elif size_bytes < 1024 * 1024:
            return f"{size_bytes / 1024:.1f} KB"
        else:
            return f"{size_bytes / (1024 * 1024):.2f} MB"
    except Exception:
        return "N/A"

def get_file_encoding(file_path):
    try:
        with open(file_path, 'rb') as f:
            raw = f.read(1024 * 1024)
            
        if not raw:
            return "UTF-8"

        # 1. BOM 헤더 검사
        if raw.startswith(b'\xef\xbb\xbf'):
            return "UTF-8 BOM"
        elif raw.startswith(b'\xff\xfe'):
            return "UTF-16LE"
        elif raw.startswith(b'\xfe\xff'):
            return "UTF-16BE"

        # 2. Null Byte(\x00) 포함 시 처리
        if b'\x00' in raw:
            try:
                raw.decode('utf-16le')
                return "UTF-16LE"
            except UnicodeDecodeError:
                pass
            try:
                raw.decode('utf-16be')
                return "UTF-16BE"
            except UnicodeDecodeError:
                pass
            return "UNKNOWN"

        # 3. UTF-8 검사
        try:
            raw.decode('utf-8')
            return "UTF-8"
        except UnicodeDecodeError:
            pass

        # 4. ANSI(CP949) 검사
        system_ansi = locale.getpreferredencoding() or 'cp949'
        try:
            raw.decode(system_ansi)
            return "ANSI"
        except (UnicodeDecodeError, LookupError):
            pass

        try:
            raw.decode('cp949')
            return "ANSI"
        except UnicodeDecodeError:
            pass

        return "UNKNOWN"

    except Exception:
        return "UNKNOWN"

def scan_directory(target_dir=".", output_file="check_encoding_result.md"):
    abs_root = os.path.abspath(target_dir)
    abs_output_path = os.path.abspath(output_file)
    
    print(f"Target Directory: {abs_root}")
    print(f"Target Extensions: {TARGET_EXTENSIONS}")
    print("Scanning files...")
    
    results = []
    file_count = 0
    
    for root, _, files in os.walk(target_dir):
        for file in files:
            # .cpp, .h 확장자 파일만 필터링
            if not file.lower().endswith(TARGET_EXTENSIONS):
                continue

            file_path = os.path.join(root, file)
            
            # 생성될 결과 마크다운 파일 자신은 검사 대상에서 제외
            if os.path.abspath(file_path) == abs_output_path:
                continue
                
            encoding = get_file_encoding(file_path)
            file_size = get_file_size_str(file_path)
            
            results.append((encoding, file_size, file_path))
            file_count += 1

    # UTF-8 인코딩으로 마크다운 결과 파일 저장
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write("# C++ File Encoding Scan Results\n\n")
        f.write(f"- **Target Directory:** `{abs_root}`\n")
        f.write(f"- **Target Extensions:** `{', '.join(TARGET_EXTENSIONS)}`\n")
        f.write(f"- **Total Files Checked:** {file_count}\n\n")
        f.write("| ENCODING | FILE SIZE | FILE PATH |\n")
        f.write("| :--- | :--- | :--- |\n")
        
        for encoding, size, path in results:
            safe_path = path.replace("|", "\\|")
            f.write(f"| {encoding} | {size} | `{safe_path}` |\n")

    print(f"Done! Saved {file_count} file results to '{output_file}'.")

if __name__ == "__main__":
    target_path = sys.argv[1] if len(sys.argv) > 1 else "."
    scan_directory(target_path)