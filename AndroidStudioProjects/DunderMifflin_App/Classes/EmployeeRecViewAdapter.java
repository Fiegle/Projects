package com.example.dm_app;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.cardview.widget.CardView;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;

public class EmployeeRecViewAdapter extends RecyclerView.Adapter<EmployeeRecViewAdapter.ViewHolder> {

    private ArrayList<EmployeeModel> employees = new ArrayList<>();

    private Context context;
    private OnEmployeeListener onEmployeeListener;
    public EmployeeRecViewAdapter(Context context, OnEmployeeListener onEmployeeListener) {
        this.context = context;
        this.onEmployeeListener = onEmployeeListener;
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.employee_list_item, parent, false);
        ViewHolder holder = new ViewHolder(view, onEmployeeListener);
        return holder;
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, final int position) {

        holder.txtName.setText(employees.get(position).getFirstName() + " " + employees.get(position).getLastName());
        holder.txtEmail.setText(employees.get(position).getEmail());
        System.out.println("GOT HERE");
        /*holder.parent.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //Toast.makeText(context, employees.get(position).getFirstName() + " " + employees.get(position).getLastName() + " Selected", Toast.LENGTH_SHORT).show();
            }
        });*/


    }

    @Override
    public int getItemCount() {
        return employees.size();
    }

    public void setEmployees(ArrayList<EmployeeModel> employees) {
        this.employees = employees;
        notifyDataSetChanged();
    }

    public class ViewHolder extends RecyclerView.ViewHolder implements View.OnClickListener {
        private TextView txtName, txtEmail;
        private final CardView parent;
        OnEmployeeListener onEmployeeListener;
        public ViewHolder(@NonNull View itemView, OnEmployeeListener onEmployeeListener) {
            super(itemView);
            txtName = itemView.findViewById(R.id.txtName);
            txtEmail = itemView.findViewById(R.id.txtEmail);
            parent = itemView.findViewById(R.id.parentList);
            this.onEmployeeListener = onEmployeeListener;

            itemView.setOnClickListener(this);
        }

        @Override
        public void onClick(View view) {
            onEmployeeListener.onEmployeeClick(getAdapterPosition());
        }
    }

    public interface OnEmployeeListener {

        void onEmployeeClick(int position);
    }

}
