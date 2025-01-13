# 编译网关

import os
import argparse
import sys
import platform

def complie(release,version,outdir):
    if (platform.system() == 'Linux'):
        cmd = "cd " + sys.path[0] + " && cd ../" + " && go build "
    else:
        cmd = "cd " + sys.path[0] +" && cd ../ && go build "
    complexMode = "-race -X main.appModle='debug'"
    if release == "true":
        complexMode = "-race -w -X main.appModle='release'"
    versionInfo = " -X main.appVersion='" + version + "'"
    if outdir:
        cmd = cmd + " -o " + os.path.join(outdir)
    cmd  = cmd +" -ldflags \"" + complexMode + versionInfo + "\""
    os.system(cmd)

def winComplieLinux(release,version,outdir): 
    cmd = "SET CGO_ENABLED=0&& SET GOOS=linux&& cd " +sys.path[0] + " && cd ../ && go build"
    complexMode = "-race -X main.appModle='debug'"
    if release == "true":
        complexMode = "-race -w -X main.appModle='release'"
    versionInfo = " -X main.appVersion='" + version + "'"
    if outdir:
        cmd = cmd + " -o " + os.path.join(outdir)
    cmd  = cmd +" -ldflags \"" + complexMode + versionInfo + "\""
    os.system(cmd)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--engine", help="引擎目录", type=str)
    parser.add_argument("--project", help="项目名", type=str)
    parser.add_argument("--platform", help="指定交叉编译平台", type=str)
    parser.add_argument("--service", help="服务名", type=str)
    parser.add_argument("--release", help="编译模式", type=str)
    parser.add_argument("--version", help="版本信息", type=str)
    parser.add_argument("--outdir", help="指定生成目录", type=str)
    args = parser.parse_args()

    if (args.platform == None or platform.system() == 'Linux'):
        complie(args.release,args.version,args.outdir)
    elif (args.platform != None):
        if (args.platform == 'Linux'):
            winComplieLinux(args.release,args.version,args.outdir)
        else:
            complie(args.release,args.version,args.outdir)

if __name__ == '__main__':
    main()
