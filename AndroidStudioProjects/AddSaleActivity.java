package com.example.dm_app;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import static java.lang.Float.parseFloat;

public class AddSaleActivity extends AppCompatActivity {

    private EditText edtClientName;
    private EditText edtSaleAmount;
    private Button btnAdd;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_add_sale);
        final String Email = getIntent().getExtras().getString("Email");
        final String Branch = getIntent().getExtras().getString("Branch");

        initialize();
        btnAdd.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                SaleModel saleModel = null;
                try{
                    Float amt = parseFloat(edtSaleAmount.getText().toString());
                    saleModel = new SaleModel(-1, Email, edtClientName.getText().toString(), amt, Branch);
                }catch(Exception e){
                    Toast.makeText(AddSaleActivity.this, "Error creating Sale", Toast.LENGTH_SHORT).show();
                    saleModel = new SaleModel();

                }

                DataBaseHelper dataBaseHelper = new DataBaseHelper(AddSaleActivity.this);
                boolean success = dataBaseHelper.addSale(saleModel);
                if(success == true){
                    Toast.makeText(AddSaleActivity.this, "Sale Added Successfully", Toast.LENGTH_LONG).show();
                    Intent intent = new Intent(AddSaleActivity.this, EmployeeActivity.class);
                    intent.putExtra("Email", Email);
                    intent.putExtra("NewEmail", Email);
                    startActivity(intent);
                }else{
                    Toast.makeText(AddSaleActivity.this, "Error Adding Sale", Toast.LENGTH_SHORT).show();
                }
            }
        });
    }

    private void initialize() {
        edtClientName = findViewById(R.id.edtClientName);
        edtSaleAmount = findViewById(R.id.edtSaleAmount);
        btnAdd = findViewById(R.id.btnAdd);

    }

}