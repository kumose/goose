# Train Benchmark queries

For the benchmark specification see the paper [The Train Benchmark: cross-technology performance evaluation of continuous model queries](https://link.springer.com/10.1007/s10270-016-0571-8) (Softw. Syst. Model. 2018)

## Usage

### Loading the data

The test data is from the benchmark's "Repair" scenario with data set size (scale factor) 1.

```bash
cat schema.sql | goose tb.goose
TB_DATA_DIR=sf1
sed -i.bkp "s|\\N||g" ${TB_DATA_DIR}/Route.csv
sed "s/PATHVAR/${TB_DATA_DIR}/g" tb-load.sql | goose tb.goose
```

### Queries

```bash
cat queries/connectedsegments.sql | goose tb.goose
cat queries/routesensor.sql | goose tb.goose
cat queries/poslength.sql | goose tb.goose
cat queries/semaphoreneighbor.sql | goose tb.goose
cat queries/switchmonitored.sql | goose tb.goose
cat queries/switchset.sql | goose tb.goose
```
