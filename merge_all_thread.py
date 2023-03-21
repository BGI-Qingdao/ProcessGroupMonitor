import pandas as pd
import numpy as np

data = pd.read_csv('tmp_pglog_all.csv',sep='\t',header=0)
data.columns = ['tip','pid','time','cpu','memory']

data['time'] = data['time']-data['time'].min()
data['time'] = data['time'] / 60
data['time'] = data['time'].astype(int)
data = data.groupby(['time','pid'])[['cpu','memory']].max().reset_index()
data.to_csv('report_pglog_all.csv',sep='\t',header=True)

data = data.groupby('time')[['cpu','memory']].sum().reset_index()
print(f'CPU max:  {data["cpu"].max()}',flush=True) 
print(f'MEM max:  {data["memory"].max()}',flush=True) 
data.to_csv('report_pglog_all_as_one.csv',sep='\t',header=True)
