import argparse
import os
import yaml

def parseYml(ymlPath):
    f = open(ymlPath,mode='r',encoding='utf-8')
    data = yaml.load(f.read(),yaml.FullLoader)
    f.close()
    return data

def ElementExists(elements,k,v):
    for val in elements:
        if val[k] & val[k] == v:
            return True
    return False

def ElementArrayExists(elements,v):
    b = False
    for val in elements:
        if val == v:
            b = True
    return b

def ymlWrite(ymlPath,inputdata):
    with open(ymlPath,'w',encoding='utf-8') as f:      #'a'代表持续写入，‘w’代表覆盖写入
        yaml.dump(inputdata,f)
    
def valueType(value,type):
    if type == "int":
        return int(value)
    if type == "str":
        return value
    if type == "str[]":
        return value.split(',')
    if type == "[str]str" | type == "ssl/ttl":
        valueList = value.split(',')
        res = {}
        for val in valueList:
            arr = val.split("=")
            res[arr[0]] = arr[1]
        return res

def WithVal(context, name,value,type):
    context[name]=valueType(value,type)
    return context

def WithMapStr(context, name,value,type):
    arr = []
    if (not ElementExists(context,name)):
        context = []
        arr = valueType(value,type)
    for key,val in arr:
        if ElementExists(context[name],key,val):
            continue
        context[name].append({key:val})

def WithArrStr(context, name,value,type):
    arr = []
    if (not ElementExists(context,name)):
        context = []
        arr = valueType(value,type)
    for val in arr:
        if ElementArrayExists(context[name],val):
            continue
        context[name].append(val)
    return context
def WithAddress(context, name,value,type):
    value =  value.split(":")
    context[name]={"ip":value[0],"port":int(value[1])}
    return context

methodList = {
    "WithID":WithVal,
    "WithIP":WithVal,
    "WithPort":WithVal,
    "WithAllowClient":WithVal,
    "WithTls":WithMapStr,
    "WithNetworkType":WithVal,
    "WithLogLevel":WithVal,
    "WithLogPath":WithVal,
    "WithInterfaceIP":WithVal,
    "WithInterfacePort":WithVal,
    "WithInterfaceTags":WithArrStr,
    "WithInterfaceMeta":WithMapStr,
    "WithInterfaceAllow":WithVal,
    "WithInterfaceTls":WithMapStr,
    "WithInterfaceClusterReplicas":WithVal,
    "WithConsulAddress":WithAddress,
    "WithGatewayTags":WithArrStr,
    "WithGatewayMeta":WithMapStr,
    "WithHealthAddress":WithAddress,
    "WithHealthInterval":WithVal,
    "WithHealthAutoAfter":WithVal,
    }
        
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--project",help="项目名", type=str)
    parser.add_argument("--serviceNodeName",   help="服务节点名", type=str)
    parser.add_argument("--name",   help="yml 参数名", type=str)
    parser.add_argument("--method", help="yml 函数名", type=str)
    parser.add_argument("--value",  help="yml 值", type=str)
    parser.add_argument("--type",   help="yml 值类型", type=str)

    args = parser.parse_args()
    if args.method == None:
        print(" method is None")
        return
    if args.name == None:
        print(" name is None")
        return
    if args.value == None:
        print(" value is None")
        return
    if args.type == None:
        print(" type is None")
        return
    args.name = args.name.replace(' ','-')
    workspace = os.getenv("GOPATH")
    if not workspace.isspace():
            workspace = os.path.join(workspace, "src")
    else:
        print("请设置GOPATH")
        return
    authorizeymlPath = os.path.join(workspace,args.project)
    authorizeymlPath = os.path.join(authorizeymlPath,"bin")
    authorizeymlPath = os.path.join(authorizeymlPath,"instances")
    authorizeymlPath = os.path.join(authorizeymlPath,args.serviceNodeName)
    authorizeymlPath = os.path.join(authorizeymlPath,"config.yml")
    if (not os.path.exists(authorizeymlPath)):
        print("config.yml不存在")
        return
    context = parseYml(authorizeymlPath)
    if methodList[args.method] == None:
        print(args.method + " 不存在")
        return 
    context = methodList[args.method](context,args.name,args.value,args.type)
    ymlWrite(authorizeymlPath,context)

if __name__ == '__main__':
    main()